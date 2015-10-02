##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=bow_matching_visualizer
ConfigurationName      :=Release
WorkspacePath          := "/home/stylix/Dropbox/MyDocument/SokendaiCourse/Researches/Workspace/code"
ProjectPath            := "/home/stylix/Dropbox/MyDocument/SokendaiCourse/Researches/Workspace/code/bow_matching_visualizer"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Siriwat Kasamwattanarote
Date                   :=02/10/15
CodeLitePath           :="/home/stylix/Dropbox/MyDocument/SokendaiCourse/Researches/Workspace/configurations/.codelite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="bow_matching_visualizer.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  `pkg-config opencv --libs` `pkg-config --libs lapacke`
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)/home/stylix/local/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)ins $(LibrarySwitch)alphautils $(LibrarySwitch)sifthesaff $(LibrarySwitch)opencv_core $(LibrarySwitch)opencv_features2d $(LibrarySwitch)opencv_highgui $(LibrarySwitch)ransac $(LibrarySwitch)lapacke $(LibrarySwitch)lapack $(LibrarySwitch)blas $(LibrarySwitch)tmglib 
ArLibs                 :=  "libins.a" "libalphautils.a" "libsifthesaff.a" "opencv_core" "opencv_features2d" "opencv_highgui" "libransac.a" "lapacke" "lapack" "blas" "tmglib" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)/home/stylix/local/lib $(LibraryPathSwitch)../lib/ins/$(ConfigurationName) $(LibraryPathSwitch)../lib/sifthesaff/$(ConfigurationName) $(LibraryPathSwitch)../lib/alphautils/$(ConfigurationName) $(LibraryPathSwitch)../lib/ransac/$(ConfigurationName) 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcu
CXX      := g++
CC       := gcc
CXXFLAGS :=  -O3 -fopenmp -std=c++11 -Wall `pkg-config --cflags opencv` $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/bow_matching_visualizer.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Release || $(MakeDirCommand) ./Release


$(IntermediateDirectory)/.d:
	@test -d ./Release || $(MakeDirCommand) ./Release

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/bow_matching_visualizer.cpp$(ObjectSuffix): bow_matching_visualizer.cpp $(IntermediateDirectory)/bow_matching_visualizer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/stylix/Dropbox/MyDocument/SokendaiCourse/Researches/Workspace/code/bow_matching_visualizer/bow_matching_visualizer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bow_matching_visualizer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bow_matching_visualizer.cpp$(DependSuffix): bow_matching_visualizer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bow_matching_visualizer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bow_matching_visualizer.cpp$(DependSuffix) -MM "bow_matching_visualizer.cpp"

$(IntermediateDirectory)/bow_matching_visualizer.cpp$(PreprocessSuffix): bow_matching_visualizer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bow_matching_visualizer.cpp$(PreprocessSuffix) "bow_matching_visualizer.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


