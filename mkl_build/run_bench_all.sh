OUT=perf.csv
rm ${OUT}

config1='g1mb28ic64ih56iw56oc256oh56ow56kh1kw1sh1sw1ph0pw0n'
config2='g1mb28ic64ih56iw56oc64oh56ow56kh1kw1sh1sw1ph0pw0n'
config3='g1mb28ic64ih56iw56oc64oh56ow56kh3kw3sh1sw1ph1pw1n'
config4='g1mb28ic256ih56iw56oc64oh56ow56kh1kw1sh1sw1ph0pw0n'
config5='g1mb28ic256ih28iw28oc512oh28ow28kh1kw1sh1sw1ph0pw0n'
config6='g1mb28ic256ih28iw28oc128oh28ow28kh1kw1sh1sw1ph0pw0n'
config7='g1mb28ic128ih28iw28oc128oh28ow28kh3kw3sh1sw1ph1pw1n'
config8='g1mb28ic128ih28iw28oc512oh28ow28kh1kw1sh1sw1ph0pw0n'
config9='g1mb28ic512ih28iw28oc128oh28ow28kh1kw1sh1sw1ph0pw0n'
config10='g1mb28ic512ih14iw14oc1024oh14ow14kh1kw1sh1sw1ph0pw0n'
config11='g1mb28ic512ih14iw14oc256oh14ow14kh1kw1sh1sw1ph0pw0n'
config12='g1mb28ic256ih14iw14oc256oh14ow14kh3kw3sh1sw1ph1pw1n'
config13='g1mb28ic256ih14iw14oc1024oh14ow14kh1kw1sh1sw1ph0pw0n'
config14='g1mb28ic1024ih14iw14oc256oh14ow14kh1kw1sh1sw1ph0pw0n'
config15='g1mb28ic1024ih7iw7oc2048oh7ow7kh1kw1sh1sw1ph0pw0n'
config16='g1mb28ic1024ih7iw7oc512oh7ow7kh1kw1sh1sw1ph0pw0n'
config17='g1mb28ic512ih7iw7oc512oh7ow7kh3kw3sh1sw1ph1pw1n'
config18='g1mb28ic512ih7iw7oc2048oh7ow7kh1kw1sh1sw1ph0pw0n'
config19='g1mb28ic2048ih7iw7oc512oh7ow7kh1kw1sh1sw1ph0pw0n'

#Mobilenet
#config20='g1mb28ic3ih224iw224oc32oh224ow224kh3kw3sh2sw2ph1pw1n'
config21='g1mb28ic32ih112iw112oc64oh112ow112kh1kw1sh1sw1ph0pw0n'
config22='g1mb28ic64ih56iw56oc128oh56ow56kh1kw1sh1sw1ph0pw0n'
config23='g1mb28ic128ih56iw56oc128oh56ow56kh1kw1sh1sw1ph0pw0n'
config24='g1mb28ic128ih28iw28oc256oh28ow28kh1kw1sh1sw1ph0pw0n'
config25='g1mb28ic256ih28iw28oc256oh28ow28kh1kw1sh1sw1ph0pw0n'
config26='g1mb28ic256ih14iw14oc512oh14ow14kh1kw1sh1sw1ph0pw0n'
config27='g1mb28ic512ih14iw14oc512oh14ow14kh1kw1sh1sw1ph0pw0n'
config28='g1mb28ic512ih7iw7oc1024oh7ow7kh1kw1sh1sw1ph0pw0n'
config29='g1mb28ic1024ih7iw7oc1024oh7ow7kh1kw1sh1sw1ph0pw0n'
config30='g1mb28ic1024ih1iw1oc5oh1ow1kh1kw1sh1sw1ph0pw0n'

#Alexnet
#config31='g1mb28ic3ih227iw227oc64oh227ow227kh11kw11sh4sw4ph0pw0n'
config32='g1mb28ic64ih27iw27oc192oh27ow27kh5kw5sh1sw1ph2pw2n'
config33='g1mb28ic192ih13iw13oc384oh13ow13kh3kw3sh1sw1ph1pw1n'
config34='g1mb28ic384ih13iw13oc256oh13ow13kh3kw3sh1sw1ph1pw1n'
config35='g1mb28ic256ih13iw13oc256oh13ow13kh3kw3sh1sw1ph1pw1n'

for config in "$config1" "$config2" "$config3" "$config4" "$config5" "$config6" "$config7" "$config8" "$config9" "$config10" "$config11" "$config12" "$config13" "$config14" "$config15" "$config16" "$config17" "$config18" "$config19" "$config21" "$config22" "$config23" "$config24" "$config25" "$config26" "$config27" "$config28" "$config29" "$config30" "$config32" "$config33" "$config34" "$config35"

do
#echo -n $config, >> ${OUT}
GFLOPS=`./benchdnn --conv --mode=p --dir=FWD_D --cfg=f32 $config"resnet_50:conv1" | grep "nresnet_50:conv1"| cut -d"," -f10`
echo -n $GFLOPS >> ${OUT}
echo "" >> ${OUT}
done
echo >> ${OUT}
