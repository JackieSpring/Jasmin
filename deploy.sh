
COMMIT_MSG=$1
EXT_RES_DIR="src/shared/external_rsc/"
KEYSTONE_PATH=$EXT_RES_DIR"keystone-master/"
CAPSTONE_PATH=$EXT_RES_DIR"capstone-next/"

KEYSTONE_ZIP=$EXT_RES_DIR"jasmin-keystone-master.zip"
CAPSTONE_ZIP=$EXT_RES_DIR"jasmin-capstone-next.zip"

if [[ -e $KEYSTONE_ZIP ]]; then
    zip -9 -r -u $KEYSTONE_ZIP $KEYSTONE_PATH
else
    zip -9 -r $KEYSTONE_ZIP $KEYSTONE_PATH
fi

if [[ -e $CAPSTONE_ZIP ]]; then
    zip -9 -r -u $CAPSTONE_ZIP $CAPSTONE_PATH
else
    zip -9 -r $CAPSTONE_ZIP $CAPSTONE_PATH
fi

#git rm -r --cached .
git add .
git commit -m "$COMMIT_MSG"
git push