#!/bin/bash

DOWNLOAD_URL=""
SILENT_ARGS="";
IS_SILENT=false;
IS_OUTPUT=false;
URL_REGEX='(https?|ftp|file)://[-A-Za-z0-9\+&@#/%?=~_|!:,.;]*[-A-Za-z0-9\+&@#/%=~_|]'


printDownloaderHelp(){
cat << EOF
    Usage: download.sh [URL] [OPTIONS]

    Example: download.sh http://ci.openframeworks.cc/libs/file.zip -s

    Options:
    -u, 
    -s, --silent                Silent download progress
    -h, --help                  Shows this message
EOF
}

downloader() { 

	if [ -z "$1" ] then printDownloaderHelp; return; fi

    if command -v curl 2>/dev/null; then 
        curl -LO --retry 20 -O --progress $@; 
    elif command -v wget 2>/dev/null; then 
        wget $@ 2> /dev/null; fi; 
    else 
        wget $@ 2> /dev/null; fi; 

    
    if [[ $1 =~ $regex ]] then 
        echo "Link valid"
    else
        echo "Link not valid"
    fi

    while getopts u:s:--silent:o:e: option
    do
    case "${option}"
    in
    u) DOWNLOAD_URL=${OPTARG};;
    s|--silent) IS_SILENT=true;;
    o|--output) IS_OUTPUT=true;;
    e) FORMAT=$OPTARG;;
    esac
    done

    if command -v curl 2>/dev/null; then 
    	curl -LO --retry 20 -O --progress $@; 
    elif command -v wget 2>/dev/null; then 
        wget $@ 2> /dev/null; fi; 
    else 
        wget $@ 2> /dev/null; fi; 
}

