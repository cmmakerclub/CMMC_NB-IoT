platformio lib -g install 73 # altsoftserial
platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=uno --board=megaatmega2560 "examples/AT-Bridge"
platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=uno --board=megaatmega2560 "examples/altsoftserial_example"
platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=megaatmega2560 "examples/hwserial_example"

