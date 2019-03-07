#!/bin/bash

# Ensure that we have only 2 arguments passed.
if [[ "$#" -ne 2 ]]; then
    echo "Usage: trial-all-methods.sh [experiment] [threads per method]"
    exit 1
fi

# Determine the path of this project, and of our config file.
HOKU_PROJECT_PATH="$(dirname "$0")/../"
HOKU_CONFIG_PATH=${HOKU_CONFIG_INI-${HOKU_PROJECT_PATH}/CONFIG.ini}

# Ensure that PerformE exists before proceeding.
if [[ ! -f ${HOKU_PROJECT_PATH}/bin/PerformE ]]; then
    echo "'PerformE' not found. Build the file 'src/perform-e.cpp'."
    exit 1
fi

# Modify the CONFIG.ini file to run the experiments in parallel.
cp ${HOKU_CONFIG_PATH} ${HOKU_CONFIG_PATH}.bak
ORIGINAL_SAMPLE_LINE=$(grep "^samples =" ${HOKU_CONFIG_PATH})
ORIGINAL_SAMPLES=$(echo ${ORIGINAL_SAMPLE_LINE} | grep -o -E '[0-9]+')
PARALLEL_SAMPLES=$(echo $(($ORIGINAL_SAMPLES / $2)) | awk '{print int($1+0.5)}')
sed -i "s/\(samples *= *\).*/\1$PARALLEL_SAMPLES/" ${HOKU_CONFIG_PATH}

# We generate a random temporary folder to store our results before we merge.
TMP_RESULTS_DIR=${HOKU_PROJECT_PATH}/data/$(head /dev/urandom | tr -dc A-Za-z0-9 | head -c 13 ; echo '')
mkdir ${TMP_RESULTS_DIR}

# We finish the results of a single method first, before proceeding to the next method.
for m in "angle" "dot" "plane" "sphere" "pyramid" "composite"; do
    echo "Running experiments for the $m method."
    for i in `seq 1 $2`; do
        ${HOKU_PROJECT_PATH}/bin/PerformE m $1 "$TMP_RESULTS_DIR/lumberjack-$m-$i.db" &
    done
    wait
done

# Put our CONFIG.ini file back to normal.
echo Cleaning Up CONFIG.ini
mv ${HOKU_CONFIG_PATH}.bak ${HOKU_CONFIG_PATH}

# Prepare our master database.
LUMBERJACK_DATABASE_NAME_LINE=$(grep "^lumberjack =" ${HOKU_CONFIG_PATH})
LUMBERJACK_DATABASE_NAME=`echo $(echo ${LUMBERJACK_DATABASE_NAME_LINE} | cut -d '=' -f 2 | cut -d ';' -f 1)`

cp $(find TMP_RESULTS_DIR -name "lumberjack-*.db" | head -1) ${HOKU_PROJECT_PATH}/${LUMBERJACK_DATABASE_NAME}
WORKING_TABLE=$(sqlite3 ${HOKU_PROJECT_PATH}/${LUMBERJACK_DATABASE_NAME} "SELECT * \
                                                                          FROM sqlite_master \
                                                                          WHERE type = 'table' \
                                                                          LIMIT 1")
sqlite3 ${HOKU_PROJECT_PATH}/${LUMBERJACK_DATABASE_NAME} "DELETE FROM $WORKING_TABLE WHERE 1 = 1;"
sleep 1

# Merge our databases together.
for f in ${TMP_RESULTS_DIR}/"lumberjack-*.db"; do
    sqlite3 ${HOKU_PROJECT_PATH}/${LUMBERJACK_DATABASE_NAME} "ATTACH '${f}' AS A; \
                                                              INSERT INTO $WORKING_TABLE \
                                                              $WORKING_TABLE \
                                                              SELECT * \
                                                              FROM A.$WORKING_TABLE; \
                                                              DETACH DATABASE A;"
done
echo "Database merging is finished."
echo "Results can be found in 'data/lumberjack.db."

