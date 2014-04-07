#./typeTest-script.sh >> out 2> out
rm -rf outputs
mkdir outputs
cd typecheck/test
for OUTPUT in $(ls in*)
do
	echo  "Processing file $OUTPUT ${OUTPUT/in/out}"
	../../parser $OUTPUT prtType >>  ../../outputs/${OUTPUT/in/out} 2> ../../outputs/${OUTPUT/in/out}
done
cd ../../

for OUTPUT in $(ls outputs/out*)
do
        echo "Processing output file $OUTPUT"
	diff ${OUTPUT/outputs/typecheck\/test} $OUTPUT
done


