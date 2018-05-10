platformio lib -g install 73 # altsoftserial

# BOARD=espresso_lite_v2 
# rm -r test 
# for EXAMPLE in $PWD/examples/*; 
# do
# 	platformio ci --project-option="lib_ldf_mode=2" $EXAMPLE -l '.' -b $BOARD
# done 

platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=uno --board=uno "examples/AT-Bridge_uno"
platformio ci --project-option="lib_ldf_mode=2" --lib="." --board=uno --board=megaatmega2560 "examples/AT-brdige_hardware_serial"

