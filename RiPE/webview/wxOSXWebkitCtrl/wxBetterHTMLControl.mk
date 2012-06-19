##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=wxBetterHTMLControl
ConfigurationName      :=Debug
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
WorkspacePath          := "/Users/mmg/Workspace_CL"
ProjectPath            := "/Users/mmg/Desktop/wxbetterhtmlcontrol/wxOSXWebkitCtrl"
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=mmg
Date                   :=09/29/10
CodeLitePath           :="/Users/mmg/Library/Application Support/codelite"
LinkerName             :=g++
ArchiveTool            :=ar rcus
SharedObjectLinkerName :=g++ -dynamiclib -fPIC
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
OutputFile             :=$(IntermediateDirectory)/$(ProjectName).app/Contents/MacOS/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)__WX__ 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
MakeDirCommand         :=mkdir -p
CmpOptions             := -g $(shell /usr/local/bin/wx-config-2.9 --cxxflags --unicode=yes --debug=yes) -arch i386 $(Preprocessors)
LinkOptions            :=  -mwindows $(shell /usr/local/bin/wx-config-2.9  --debug=yes --unicode=yes  --libs core,base,net,stc) -arch i386
IncludePath            :=  "$(IncludeSwitch)." "$(IncludeSwitch)/Users/mmg/Desktop/wxbetterhtmlcontrol" 
RcIncludePath          :=
Libs                   :=
LibPath                := "$(LibraryPathSwitch)." 


##
## User defined environment variables
##
CodeLiteDir:=/Applications/CodeLite.app/Contents/SharedSupport/
DYLD_LIBRARY_PATH:=/Developer/hg/mupen64plus/mupen64plus-core/projects/unix
Objects=$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(ObjectSuffix) $(IntermediateDirectory)/webkit$(ObjectSuffix) $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(ObjectSuffix) $(IntermediateDirectory)/wxIETest2_wxiepanel$(ObjectSuffix) $(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(ObjectSuffix) 

##
## Main Build Targets 
##
all: $(OutputFile)

$(OutputFile): makeDirStep $(Objects)
	@$(MakeDirCommand) $(@D)
	$(LinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)

makeDirStep:
	@test -d ./Debug || $(MakeDirCommand) ./Debug
PrePreBuild: $(IntermediateDirectory)/$(ProjectName).app/Contents/Info.plist $(IntermediateDirectory)/$(ProjectName).app/Contents/Resources/wxBetterHTMLControl.icns
## rule to copy the Info.plist file into the bundle
$(IntermediateDirectory)/$(ProjectName).app/Contents/Info.plist: Info.plist
	mkdir -p '$(IntermediateDirectory)/$(ProjectName).app/Contents' && cp -f Info.plist '$(IntermediateDirectory)/$(ProjectName).app/Contents/Info.plist'
## rule to copy the icon file into the bundle
$(IntermediateDirectory)/$(ProjectName).app/Contents/Resources/wxBetterHTMLControl.icns: wxBetterHTMLControl.icns
	mkdir -p '$(IntermediateDirectory)/$(ProjectName).app/Contents/Resources/' && cp -f wxBetterHTMLControl.icns '$(IntermediateDirectory)/$(ProjectName).app/Contents/Resources/wxBetterHTMLControl.icns'

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(ObjectSuffix): ../main.cpp $(IntermediateDirectory)/wxbetterhtmlcontrol_main$(DependSuffix)
	$(CompilerName) $(SourceSwitch) "/Users/mmg/Desktop/wxbetterhtmlcontrol/main.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(DependSuffix): ../main.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(ObjectSuffix) -MF$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(DependSuffix) -MM "/Users/mmg/Desktop/wxbetterhtmlcontrol/main.cpp"

$(IntermediateDirectory)/wxbetterhtmlcontrol_main$(PreprocessSuffix): ../main.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxbetterhtmlcontrol_main$(PreprocessSuffix) "/Users/mmg/Desktop/wxbetterhtmlcontrol/main.cpp"

$(IntermediateDirectory)/webkit$(ObjectSuffix): webkit.mm $(IntermediateDirectory)/webkit$(DependSuffix)
	$(CompilerName) -x objective-c++ $(SourceSwitch) "/Users/mmg/Desktop/wxbetterhtmlcontrol/wxOSXWebkitCtrl/webkit.mm" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/webkit$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/webkit$(DependSuffix): webkit.mm
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/webkit$(ObjectSuffix) -MF$(IntermediateDirectory)/webkit$(DependSuffix) -MM "/Users/mmg/Desktop/wxbetterhtmlcontrol/wxOSXWebkitCtrl/webkit.mm"

$(IntermediateDirectory)/webkit$(PreprocessSuffix): webkit.mm
	@$(CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/webkit$(PreprocessSuffix) "/Users/mmg/Desktop/wxbetterhtmlcontrol/wxOSXWebkitCtrl/webkit.mm"

$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(ObjectSuffix): ../webview.cpp $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(DependSuffix)
	$(CompilerName) $(SourceSwitch) "/Users/mmg/Desktop/wxbetterhtmlcontrol/webview.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(DependSuffix): ../webview.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(ObjectSuffix) -MF$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(DependSuffix) -MM "/Users/mmg/Desktop/wxbetterhtmlcontrol/webview.cpp"

$(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(PreprocessSuffix): ../webview.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(PreprocessSuffix) "/Users/mmg/Desktop/wxbetterhtmlcontrol/webview.cpp"

$(IntermediateDirectory)/wxIETest2_wxiepanel$(ObjectSuffix): ../wxIETest2/wxiepanel.cpp $(IntermediateDirectory)/wxIETest2_wxiepanel$(DependSuffix)
	$(CompilerName) $(SourceSwitch) "/Users/mmg/Desktop/wxbetterhtmlcontrol/wxIETest2/wxiepanel.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/wxIETest2_wxiepanel$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxIETest2_wxiepanel$(DependSuffix): ../wxIETest2/wxiepanel.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/wxIETest2_wxiepanel$(ObjectSuffix) -MF$(IntermediateDirectory)/wxIETest2_wxiepanel$(DependSuffix) -MM "/Users/mmg/Desktop/wxbetterhtmlcontrol/wxIETest2/wxiepanel.cpp"

$(IntermediateDirectory)/wxIETest2_wxiepanel$(PreprocessSuffix): ../wxIETest2/wxiepanel.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxIETest2_wxiepanel$(PreprocessSuffix) "/Users/mmg/Desktop/wxbetterhtmlcontrol/wxIETest2/wxiepanel.cpp"

$(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(ObjectSuffix): ../wxGtkWebKitCtrl/wxgtkwebkitctrl.cpp $(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(DependSuffix)
	$(CompilerName) $(SourceSwitch) "/Users/mmg/Desktop/wxbetterhtmlcontrol/wxGtkWebKitCtrl/wxgtkwebkitctrl.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(DependSuffix): ../wxGtkWebKitCtrl/wxgtkwebkitctrl.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(ObjectSuffix) -MF$(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(DependSuffix) -MM "/Users/mmg/Desktop/wxbetterhtmlcontrol/wxGtkWebKitCtrl/wxgtkwebkitctrl.cpp"

$(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(PreprocessSuffix): ../wxGtkWebKitCtrl/wxgtkwebkitctrl.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(PreprocessSuffix) "/Users/mmg/Desktop/wxbetterhtmlcontrol/wxGtkWebKitCtrl/wxgtkwebkitctrl.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_main$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/webkit$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/webkit$(DependSuffix)
	$(RM) $(IntermediateDirectory)/webkit$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(DependSuffix)
	$(RM) $(IntermediateDirectory)/wxbetterhtmlcontrol_webview$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/wxIETest2_wxiepanel$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/wxIETest2_wxiepanel$(DependSuffix)
	$(RM) $(IntermediateDirectory)/wxIETest2_wxiepanel$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(DependSuffix)
	$(RM) $(IntermediateDirectory)/wxGtkWebKitCtrl_wxgtkwebkitctrl$(PreprocessSuffix)
	$(RM) $(OutputFile)


