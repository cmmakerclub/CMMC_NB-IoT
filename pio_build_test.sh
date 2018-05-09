platformio lib -g install 73 # altsoftserial

# BOARD=espresso_lite_v2 
# rm -r test 
# for EXAMPLE in $PWD/examples/*; 
# do
# 	platformio ci --project-option="lib_ldf_mode=2" $EXAMPLE -l '.' -b $BOARD
# done 

platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=uno --board=megaatmega2560 "examples/AT-Bridge"
platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=uno --board=megaatmega2560 "examples/altsoftserial_example"
platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=megaatmega2560 "examples/hwserial_example"

