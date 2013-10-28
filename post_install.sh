#!/bin/sh
CUR_USER=`logname`
INPUT_GROUP=input
UDEV_RULE="SUBSYSTEM==\"input\", MODE=\"0660\", GROUP=\"${INPUT_GROUP}\""
UDEV_RULES_DIR=/etc/udev/rules.d
UDEV_RULES_FILE="${UDEV_RULES_DIR}/85-linux_input_system.rules"
if [ -f "${UDEV_RULES_FILE}" ]; then
	mv "${UDEV_RULES_FILE}" "${UDEV_RULES_FILE}.old"
fi
( \
	groupadd -f "${INPUT_GROUP}" && \
	usermod -aG "${INPUT_GROUP}" "${CUR_USER}" && \
	(echo "${UDEV_RULE}" | sudo tee "${UDEV_RULES_FILE}" > /dev/null ) && \
	echo "User \"${CUR_USER}\" was successfuly added to group \"${INPUT_GROUP}\"" || \
	echo "Failed to add user \"${CUR_USER}\" to group \"${INPUT_GROUP}\"" \
)

