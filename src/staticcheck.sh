#!/bin/bash


set -e


SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

TMPDIR=$(dirname $(mktemp -u))


checkConfig=0

for i in "$@"; do
case $i in
    --check-config|-cc)     ## check config
                        	checkConfig=1
                        	;;
    *)                  	;;                      # unknown option, ignore
esac
done

 

REPORT_DIR=$TMPDIR/memorymap/cppcheck
mkdir -p $REPORT_DIR


cppcheckAdditionalParams=""
if [ $checkConfig -eq 1 ]; then
    cppcheckAdditionalParams="--check-config "
fi


cppcheck --enable=all $cppcheckAdditionalParams \
         --suppress=missingIncludeSystem \
	     -I $SCRIPT_DIR/rbtree/include \
	     -I $SCRIPT_DIR/memorymap/include \
	     -I $SCRIPT_DIR/mymap/include \
	     --xml --xml-version=2 $SCRIPT_DIR 2> $REPORT_DIR/report.xml


$SCRIPT_DIR/../htmlreport/cppcheck-htmlreport --source-dir=$SCRIPT_DIR --title="MemoryMap src" --file=$REPORT_DIR/report.xml --report-dir=$REPORT_DIR
