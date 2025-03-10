isDebugBuild=true
cleanBuild=false
runAfterBuild=false
directoryName=""
programName="NestWatcher"
threadCount=4

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
	./$programName
fi

