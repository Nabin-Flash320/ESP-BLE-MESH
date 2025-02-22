

usb_device=$1

if [ -e "$usb_device" ]; then
    if [ ! -n "$IDF_PATH" ]; then
        source ~/esp/v5.3/esp-idf/export.sh
    fi
    idf.py -p $usb_device monitor
else 
    echo "No USB provided"
fi

