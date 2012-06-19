##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=wxIETest2
ConfigurationName      :=Debug
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
WorkspacePath          := "C:\Documents and Settings\mmg\Desktop\CLWorkspace"
ProjectPath            := "C:\Documents and Settings\mmg\Desktop\wxbetterhtmlcontrol\wxIETest2"
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=mmg
Date                   :=09/27/10
CodeLitePath           :="C:\Program Files\CodeLite"
LinkerName             :=g++
ArchiveTool            :=ar rcus
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
CompilerName           :=g++
C_CompilerName         :=gcc
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)__WX__ 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
MakeDirCommand         :=makedir
CmpOptions             := -g $(shell wx-config --cxxflags --unicode=yes --debug=yes) $(Preprocessors)
RcCmpOptions           := $(shell wx-config --rcflags)
RcCompilerName         :=windres
LinkOptions            :=  -mwindows $(shell wx-config --debug=yes --libs --unicode=yes core,base,net,stc) ./Debug/iewin.rc.o
IncludePath            :=  "$(IncludeSwitch)." "$(IncludeSwitch).." 
RcIncludePath          :=
Libs                   :=
LibPath                := "$(LibraryPathSwitch)." 


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
UNIT_TEST_PP_SRC_DIR:=C:\UnitTest++-1.3
WXWIN:=C:\wxWidgets-2.9.1
WXCFG:=gcc_dll\mswud
Objects=$(IntermediateDirectory)/wxiepanel$(ObjectSuffix) $(IntermediateDirectory)/wxbetterhtmlcontrol_main$(ObjectSuffix) $(IntermediateDirectory)/iewin.rc$(ObjectSuffix) $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(ObjectSuffix) 

##
## Main Build Targets 
##
all: $(OutputFile)

$(OutputFile): makeDirStep $(Objects)
	@$(MakeDirCommand) $(@D)
	$(LinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)

makeDirStep:
	@$(MakeDirCommand) "./Debug"

PreBuild:
	@echo Executing Pre Build commands ...
	windres -i "./iewin.rc" --use-temp-file --define __WXMSW__ --define __WXDEBUG__ --define _UNICODE --include-dir C:\wxWidgets-2.9.1\lib\gcc_dll\mswud --include-dir C:\wxWidgets-2.9.1\include --define WXUSINGDLL -o ./Debug/iewin.rc.o
	@echo Done


##
## Objects
##
$(IntermediateDirectory)/wxiepanel$(ObjectSuffix): wxiepanel.cpp $(IntermediateDirectory)/wxiepanel$(DependSuffix)
	$(CompilerName) $(SourceSwitch) "C:/Documents and Settings/mmg/Desktop/wxbetterhtmlcontrol/wxIETest2/wxiepanel.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/wxiepanel$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxiepanel$(DependSuffix): wxiepanel.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/wxiepanel$(ObjectSuffix) -MF$(IntermediateDirectory)/wxiepanel$(DependSuffix) -MM "C:/Documents and Settings/mmg/Desktop/wxbetterhtmlcontrol/wxIETest2/wxiepanel.cpp"

$(IntermediateDirectory)/wxiepanel$(PreprocessSuffix): wxiepanel.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxiepanel$(PreprocessSuffix) "C:/Documents and Settings/mmg/Desktop/wxbetterhtmlcontrol/wxIETest2/wxiepanel.cpp"

$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(ObjectSuffix): ../main.cpp $(IntermediateDirectory)/wxbetterhtmlcontrol_main$(DependSuffix)
	$(CompilerName) $(SourceSwitch) "C:/Documents and Settings/mmg/Desktop/wxbetterhtmlcontrol/main.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(DependSuffix): ../main.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(ObjectSuffix) -MF$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(DependSuffix) -MM "C:/Documents and Settings/mmg/Desktop/wxbetterhtmlcontrol/main.cpp"

$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(PreprocessSuffix): ../main.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxbetterhtmlcontrol_main$(PreprocessSuffix) "C:/Documents and Settings/mmg/Desktop/wxbetterhtmlcontrol/main.cpp"

$(IntermediateDirectory)/iewin.rc$(ObjectSuffix): iewin.rc
	$(RcCompilerName) -i "C:/Documents and Settings/mmg/Desktop/wxbetterhtmlcontrol/wxIETest2/iewin.rc" $(RcCmpOptions)   $(ObjectSwitch)$(IntermediateDirectory)/iewin.rc$(ObjectSuffix) $(RcIncludePath)
$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(ObjectSuffix): ../webview.cpp $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(DependSuffix)
	$(CompilerName) $(SourceSwitch) "C:/Documents and Settings/mmg/Desktop/wxbetterhtmlcontrol/webview.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(DependSuffix): ../webview.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(ObjectSuffix) -MF$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(DependSuffix) -MM "C:/Documents and Settings/mmg/Desktop/wxbetterhtmlcontrol/webview.cpp"

$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(PreprocessSuffix): ../webview.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(PreprocessSuffix) "C:/Documents and Settings/mmg/Desktop/wxbetterhtmlcontrol/webview.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/wxiepanel$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/wxiepanel$(DependSuffix)
	$(RM) $(IntermediateDirectory)/wxiepanel$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_main$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/iewin.rc$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(DependSuffix)
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile).exe


