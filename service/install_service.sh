#!/bin/bash



echo "-- INSTALL GM70REST SERVICE ----"
cd "$(dirname "$0")"

 CWD="$(pwd)"

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

if [[ -z "${GM70REST_INSTALL_DIR}" ]]; then
  export GM70REST_INSTALL_DIR=/usr/gm70rest
fi

echo "INSTALLATION DIR"
echo "${GM70REST_INSTALL_DIR}"


# CREATE SERVICE FILE
pwd
cp ./gm70rest.service.TEMPLATE ./gm70rest.service
sed -i -e 's|_GM70REST_INSTALL_DIR_|'$GM70REST_INSTALL_DIR'|' ./gm70rest.service


# INSTALL SERVICE
cp ./gm70rest.service /etc/systemd/system/
cat /etc/systemd/system/gm70rest.service
systemctl daemon-reload



# MODIFY CAN2REST START FILE
cp ./start_gm70rest.sh.TEMPLATE ./start_gm70rest.sh
chmod +x ./start_gm70rest.sh
sed -i -e 's|_GM70REST_INSTALL_DIR_|'$GM70REST_INSTALL_DIR'|' ./start_gm70rest.sh
cat ./start_gm70rest.sh



# COPY UDEV RULE
cp ./99-gm70rest.rules /etc/udev/rules.d
udevadm control --reload-rules
udevadm trigger
# BUILD
bash ../src/build.sh

echo $CWD
cd $CWD


# FINALLY COPY ALL OVER
mkdir -p $GM70REST_INSTALL_DIR
cp ../src/gm70rest $GM70REST_INSTALL_DIR/gm70rest
cp ./start_gm70rest.sh $GM70REST_INSTALL_DIR/start_gm70rest.sh




# ENABLE SERVICE
systemctl enable gm70rest
sleep 10
systemctl status gm70rest
