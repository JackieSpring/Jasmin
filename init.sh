
DIST_PATH="dist/"
EXE_FILE=$DIST_PATH"jasmin"
LINK_PATH="./"
LINK_FILE=$LINK_PATH"jasmin"
EXT_RES_DIR="src/shared/external_rsc/"

INFO="[ INFO ]"
ERR="[ ERR ]"

if [[ ! -e dist/ ]]; then
    mkdir dist
else
    echo "$ERR Cannot create dist/ directory!"
    exit
fi
if [[ ! -e src/shared/external_include ]]; then
    mkdir dist
else
    echo "$ERR Canno create src/shared/external_include directory!"
    exit
fi
### Check commands

if [[ ! $(which unzip) ]]; then
    echo "$ERR 'unzip' command not found! Install unzip then retry."
    exit

elif [[ ! $(which make) ]]; then
    echo "$ERR 'make' command not found! Install make then retry."
    exit

elif [[ ! $(which cmake) ]]; then
    echo "$ERR 'cmake' command not found! Install cmake then retry."
    exit
fi

### Unzip libraries

echo "$INFO Decompressing libraries"
if [[ -e $EXT_RES_DIR"capstone-next" && -e $EXT_RES_DIR"keystone-master" ]]; then
    echo "$INFO Libraries already decompressed"
else
    unzip $EXT_RES_DIR"jasmin-capstone-next.zip" # -d $EXT_RES_DIR
    unzip $EXT_RES_DIR"jasmin-keystone-master.zip" # -d $EXT_RES_DIR
fi

### Compile libraries

echo "$INFO Compiling libraries."
if [[ ! $(./compile_capstone.sh) ]]; then
    echo "$ERR An error occurred while compiling Capstone library."
    exit
fi
if [[ ! $(./compile_keystone.sh) ]]; then
    echo "$ERR An error occurred while compiling Keystone library."
    exit
fi

if [[ -e keystone-master ]]; then  # Unknown bug, keystone-master directory is created in .
    rm -rdf keystone-master
fi

### Compile Jasmin

echo "$INFO Compiling jasmin"
make

echo "$INFO Linking Executable"
if [[ -e $LINK_FILE ]]; then
    rm $LINK_FILE
fi
if [[ ! $(ln -s $EXE_FILE $LINK_FILE) -eq 0 ]]; then
    echo "$ERR An error occurred while linking executable."
    exit
fi

echo "$INFO Done"
echo "$INFO Run jasmin with './jasmin'"
