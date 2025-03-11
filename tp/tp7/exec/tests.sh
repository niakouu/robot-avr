#!/bin/bash
set -m

cd "$(dirname "$0")"

trap 'kill $(jobs -p)' EXIT

SERIEVIAUSB=../../../serieViaUSB/serieViaUSB
AVRDUDE=avrdude
AVRDUDE_PROGRAMMER_ID=usbasp
MCU=atmega324pa
DESIREDCHAR=1

$SERIEVIAUSB -l &

# reset the device
sleep 3
$AVRDUDE -c $AVRDUDE_PROGRAMMER_ID -p $MCU -P usb 1>&2 2>/dev/null

TMP=$(mktemp)
echo -en $DESIREDCHAR > $TMP

sleep 1
echo $DESIREDCHAR
$SERIEVIAUSB -e -f $TMP 1>&2 2>/dev/null

sleep 1
echo $DESIREDCHAR
$SERIEVIAUSB -e -f $TMP 1>&2 2>/dev/null

wait