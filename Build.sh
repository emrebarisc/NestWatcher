isDebugBuild=true
cleanBuild=false
runAfterBuild=false
directoryName=""
programName="NestWatcher"
threadCount=4
runAsSudo=false

for argument in "$@"
do
    if [[ "$argument" == "release" ]]; then
    	isDebugBuild=false
    else
	    if [[ "$argument" == "run" ]]; then
	    	runAfterBuild=true
	    else
		    if [[ "$argument" == "clean" ]]; then
		    	cleanBuild=true
		else
			if [[ "$argument" == "sudo" ]]; then
				runAsSudo=true
			fi
		    fi
	    fi
    fi
done

if [ "$isDebugBuild" = true ]; then
	directoryName="Build_Debug"

	rm $directoryName/$programName
	
	if [ "$cleanBuild" = true ]; then
		rm -rf $directoryName && mkdir $directoryName && cd $directoryName && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j $threadCount
	else
		if ! [ -d $directoryName ]; then
			mkdir $directoryName
		fi
		cd $directoryName && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j $threadCount
	fi
else
	directoryName="Build_Release"

	rm $directoryName/$programName
	
	if [ "$cleanBuild" = true ]; then
		rm -rf $directoryName && mkdir $directoryName && cd $directoryName && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j $threadCount
	else
		if ! [ -d $directoryName ]; then
			mkdir $directoryName
		fi
		cd $directoryName && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j $threadCount
	fi
fi

if [ "$runAfterBuild" = true ]; then
	if [ "$runAsSudo" = true ]; then
		sudo ./$programName
	else
		./$programName
	fi
fi
