##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=assign2
ConfigurationName      :=Debug
WorkspacePath          := "/Users/Patipat/Documents/CS525/cs525/assign2"
ProjectPath            := "/Users/Patipat/Documents/CS525/cs525/assign2"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Patipat Duangchalomnin
Date                   :=02/03/2016
CodeLitePath           :="/Users/Patipat/Library/Application Support/codelite"
LinkerName             :=/usr/bin/clang++
SharedObjectLinkerName :=/usr/bin/clang++ -dynamiclib -fPIC
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
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="assign2.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -O0
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch). $(LibraryPathSwitch)Debug 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/clang++
CC       := /usr/bin/clang
CXXFLAGS :=  -g -Wall $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/Users/Patipat/Downloads/codelite.app/Contents/SharedSupport/
Objects0=$(IntermediateDirectory)/buffer_mgr.c$(ObjectSuffix) $(IntermediateDirectory)/buffer_mgr_stat.c$(ObjectSuffix) $(IntermediateDirectory)/dberror.c$(ObjectSuffix) $(IntermediateDirectory)/storage_mgr.c$(ObjectSuffix) $(IntermediateDirectory)/test_assign2_1.c$(ObjectSuffix) 



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
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/buffer_mgr.c$(ObjectSuffix): buffer_mgr.c $(IntermediateDirectory)/buffer_mgr.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/Users/Patipat/Documents/CS525/cs525/assign2/buffer_mgr.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/buffer_mgr.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/buffer_mgr.c$(DependSuffix): buffer_mgr.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/buffer_mgr.c$(ObjectSuffix) -MF$(IntermediateDirectory)/buffer_mgr.c$(DependSuffix) -MM "buffer_mgr.c"

$(IntermediateDirectory)/buffer_mgr.c$(PreprocessSuffix): buffer_mgr.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/buffer_mgr.c$(PreprocessSuffix) "buffer_mgr.c"

$(IntermediateDirectory)/buffer_mgr_stat.c$(ObjectSuffix): buffer_mgr_stat.c $(IntermediateDirectory)/buffer_mgr_stat.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/Users/Patipat/Documents/CS525/cs525/assign2/buffer_mgr_stat.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/buffer_mgr_stat.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/buffer_mgr_stat.c$(DependSuffix): buffer_mgr_stat.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/buffer_mgr_stat.c$(ObjectSuffix) -MF$(IntermediateDirectory)/buffer_mgr_stat.c$(DependSuffix) -MM "buffer_mgr_stat.c"

$(IntermediateDirectory)/buffer_mgr_stat.c$(PreprocessSuffix): buffer_mgr_stat.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/buffer_mgr_stat.c$(PreprocessSuffix) "buffer_mgr_stat.c"

$(IntermediateDirectory)/dberror.c$(ObjectSuffix): dberror.c $(IntermediateDirectory)/dberror.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/Users/Patipat/Documents/CS525/cs525/assign2/dberror.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dberror.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dberror.c$(DependSuffix): dberror.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dberror.c$(ObjectSuffix) -MF$(IntermediateDirectory)/dberror.c$(DependSuffix) -MM "dberror.c"

$(IntermediateDirectory)/dberror.c$(PreprocessSuffix): dberror.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dberror.c$(PreprocessSuffix) "dberror.c"

$(IntermediateDirectory)/storage_mgr.c$(ObjectSuffix): storage_mgr.c $(IntermediateDirectory)/storage_mgr.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/Users/Patipat/Documents/CS525/cs525/assign2/storage_mgr.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/storage_mgr.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/storage_mgr.c$(DependSuffix): storage_mgr.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/storage_mgr.c$(ObjectSuffix) -MF$(IntermediateDirectory)/storage_mgr.c$(DependSuffix) -MM "storage_mgr.c"

$(IntermediateDirectory)/storage_mgr.c$(PreprocessSuffix): storage_mgr.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/storage_mgr.c$(PreprocessSuffix) "storage_mgr.c"

$(IntermediateDirectory)/test_assign2_1.c$(ObjectSuffix): test_assign2_1.c $(IntermediateDirectory)/test_assign2_1.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/Users/Patipat/Documents/CS525/cs525/assign2/test_assign2_1.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/test_assign2_1.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/test_assign2_1.c$(DependSuffix): test_assign2_1.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/test_assign2_1.c$(ObjectSuffix) -MF$(IntermediateDirectory)/test_assign2_1.c$(DependSuffix) -MM "test_assign2_1.c"

$(IntermediateDirectory)/test_assign2_1.c$(PreprocessSuffix): test_assign2_1.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/test_assign2_1.c$(PreprocessSuffix) "test_assign2_1.c"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


