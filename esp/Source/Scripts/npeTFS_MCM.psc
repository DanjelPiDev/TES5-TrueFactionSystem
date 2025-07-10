Scriptname npeTFS_MCM extends SKI_ConfigBase

Import npeTFS_NativeFunctions

; ----------------------------------------------------------------
; MCM Menu for TrueFactionSystem - Improved Layout version: 0.5.0
; ----------------------------------------------------------------

; -------- GLOBAL VARS  --------
float timeToLoseDetection
float detectionThreshold
float detectionRadius

bool useFOVCheck
bool useLOSCheck

; -------- PRIVATE VARS --------
Armor[] wornArmors
Faction[] availableFactions
int[] _wornArmorMenuOIDs
int[] _availableFactionsMenuOIDs
int[] _factionKeywordAssignementsOIDs
int selectedArmorIndex = -1
int selectedFactionIndex = -1
int selectedKeywordFactionIndex = -1
int wornArmorCount = 0

int selectedKeywordIndex = 0

string[] availableKeywordNames
Int[] availableKeywordFormIDs

int MAX_FACTIONS = 50 ; Because papyrus does not like dynamic arrays, I have to limit it (Imagine beeing a const)
int currentFactionCount = 0 ; track assigned factions count

; Menu OIDs
int _keywordDropdownOID
int _addKeywordTextOptionOID
int _removeKeywordTextOptionOID
int _resetModTextOptionOID
int _addFactionOptionOID
int _removeFactionKeywordAssignementOID
int _timeSliderOID
int _detectionThresholdSliderOID
int _detectionRadiusOID
int _useFOVOptionOID
int _useLOSOptionOID

; Assigned manage arrays
string[] assignedKeywordsManage
Faction[] assignedFactionsManage

; Page names
string playerInformationPageName = "$TFS_Player_Information"
string armorKeywordSettingPageName = "$TFS_Armor_Keyword_Settings"
string factionManagementPageName = "$TFS_Faction_Disguise_Setup"
string factionOverviewPageName = "$TFS_Disguise_Assignments"
string modSettingsPageName = "$TFS_General_Settings"

Event OnConfigInit()
    ; Load icon for menu (Still not working)
    LoadCustomContent("skyui/TrueFactionSystem/TFS.dds", 120, -33)

    ; Initialize data arrays
    wornArmors = GetWornArmors(Game.GetPlayer())
    ; availableFactions = GetAllFactions()
    _wornArmorMenuOIDs = new int[100]
    _availableFactionsMenuOIDs = new int[100]
    _factionKeywordAssignementsOIDs = new int[100]

    InitCustomKeywords()
    InitAvailableFactions()

    ; Count how many valid armors the player is wearing
    wornArmorCount = 0
    int index = 0
    while index < wornArmors.Length
        if wornArmors[index] != None
            wornArmorCount += 1
        endif
        index += 1
    endWhile

    ; Define pages for the MCM
    Pages = new string[5]
    Pages[0] = playerInformationPageName
    Pages[1] = armorKeywordSettingPageName
    Pages[2] = factionManagementPageName
    Pages[3] = factionOverviewPageName
    Pages[4] = modSettingsPageName

    ; Load persistent settings
    timeToLoseDetection = GetTimeToLoseDetection()
    detectionThreshold = GetDetectionThreshold() * 100
    detectionRadius = GetDetectionRadius()

    useFOVCheck = GetUseFOVCheck()
    useLOSCheck = GetUseLineOfSightCheck()
endEvent

Function InitWornArmor()
    wornArmors = GetWornArmors(Game.GetPlayer())
    _wornArmorMenuOIDs = new int[100]
endFunction

Function InitAssignedKeywordFactionPair()
    assignedKeywordsManage = GetAssignedKeywords()
    assignedFactionsManage = GetAssignedFactions()
endFunction

Function InitAvailableFactions()
    availableFactions = GetAllFactions()
    _availableFactionsMenuOIDs = new int[100]
endFunction

Function InitCustomKeywords()
    ; Cannot use MAX_FACTIONS for array init, because of reasons.
    availableKeywordNames = new string[50]
    availableKeywordFormIDs = new int[50]

    ; Get all my custom keywords by name, because the formid changes with the load order...SAD

    availableKeywordNames[0] = "Bandit Faction"
    availableKeywordFormIDs[0] = GetKeywordByEditorID("npeBanditFaction").GetFormID()

    availableKeywordNames[1] = "Blades Faction"
    availableKeywordFormIDs[1] = GetKeywordByEditorID("npeBladesFaction").GetFormID()

    availableKeywordNames[2] = "Imperial Faction"
    availableKeywordFormIDs[2] = GetKeywordByEditorID("npeImperialFaction").GetFormID()

    availableKeywordNames[3] = "Companions Faction"
    availableKeywordFormIDs[3] = GetKeywordByEditorID("npeCompanionsFaction").GetFormID()

    availableKeywordNames[4] = "Dawnstar Faction"
    availableKeywordFormIDs[4] = GetKeywordByEditorID("npeDawnstarFaction").GetFormID()

    availableKeywordNames[5] = "Falkreath Faction"
    availableKeywordFormIDs[5] = GetKeywordByEditorID("npeFalkreathFaction").GetFormID()

    availableKeywordNames[6] = "Forsworn Faction"
    availableKeywordFormIDs[6] = GetKeywordByEditorID("npeForswornFaction").GetFormID()

    availableKeywordNames[7] = "Markarth Faction"
    availableKeywordFormIDs[7] = GetKeywordByEditorID("npeMarkarthFaction").GetFormID()

    availableKeywordNames[8] = "Morthal Faction"
    availableKeywordFormIDs[8] = GetKeywordByEditorID("npeMorthalFaction").GetFormID()

    availableKeywordNames[9] = "Nightingale Faction"
    availableKeywordFormIDs[9] = GetKeywordByEditorID("npeNightingaleFaction").GetFormID()

    availableKeywordNames[10] = "Riften Faction"
    availableKeywordFormIDs[10] = GetKeywordByEditorID("npeRiftenFaction").GetFormID()

    availableKeywordNames[11] = "SilverHand Faction"
    availableKeywordFormIDs[11] = GetKeywordByEditorID("npeSilverHandFaction").GetFormID()

    availableKeywordNames[12] = "Solitude Faction"
    availableKeywordFormIDs[12] = GetKeywordByEditorID("npeSolitudeFaction").GetFormID()

    availableKeywordNames[13] = "Stormcloak Faction"
    availableKeywordFormIDs[13] = GetKeywordByEditorID("npeStormcloakFaction").GetFormID()

    availableKeywordNames[14] = "Thalmor Faction"
    availableKeywordFormIDs[14] = GetKeywordByEditorID("npeThalmorFaction").GetFormID()

    availableKeywordNames[15] = "Whiterun Faction"
    availableKeywordFormIDs[15] = GetKeywordByEditorID("npeWhiterunFaction").GetFormID()

    availableKeywordNames[16] = "Windhelm Faction"
    availableKeywordFormIDs[16] = GetKeywordByEditorID("npeWindhelmFaction").GetFormID()

    availableKeywordNames[17] = "Winterhold Faction"
    availableKeywordFormIDs[17] = GetKeywordByEditorID("npeWinterholdFaction").GetFormID()

    availableKeywordNames[18] = "Full Covered Face"
    availableKeywordFormIDs[18] = GetKeywordByEditorID("npeCoveredFace").GetFormID()

    currentFactionCount = 19

    int index = currentFactionCount
    while index < MAX_FACTIONS
        availableKeywordNames[index] = ""
        availableKeywordFormIDs[index] = 0
        index += 1
    endWhile
endFunction

Function AddNewFaction(string factionName, Keyword factionKeyword)
    if currentFactionCount >= MAX_FACTIONS
        Debug.Notification("Cannot add more factions. Limit reached.")
        return
    endif

    ; Add the new faction and its keyword
    availableKeywordNames[currentFactionCount] = factionName
    availableKeywordFormIDs[currentFactionCount] = factionKeyword.GetFormID()
    currentFactionCount += 1

    Debug.Notification("Added faction: " + factionName)
EndFunction

Function RemoveNewFaction(string factionName, Keyword factionKeyword)
    int removeIndex = -1
    
    int index = 0
    bool break = false
    while !break && index < currentFactionCount
        if availableKeywordNames[index] == factionName
            removeIndex = index
            break = true
        endif
        index += 1
    endWhile

    if removeIndex != -1
        while removeIndex < currentFactionCount - 1
            availableKeywordNames[removeIndex] = availableKeywordNames[removeIndex + 1]
            availableKeywordFormIDs[removeIndex] = availableKeywordFormIDs[removeIndex + 1]
            removeIndex += 1
        endWhile

        availableKeywordNames[currentFactionCount - 1] = ""
        availableKeywordFormIDs[currentFactionCount - 1] = 0
        currentFactionCount -= 1

        Debug.Notification("Removed faction: " + factionName)
    else
        Debug.Notification("Faction not found: " + factionName)
    endif
EndFunction


; Function to retrieve keywords associated with a given armor
Keyword[] Function GetArmorKeywords(Armor akArmor)
    if akArmor == None
        return None
    endif

    ; Retrieve the keywords associated with the armor
    Keyword[] keywords = akArmor.GetKeywords()
    return keywords
EndFunction

; Function to retrieve all the armor items the player is currently wearing
Armor[] Function GetWornArmors(Actor target)
    Armor[] wornArmorForms = new Armor[30] ; Max size to handle most scenarios
    int index
    int slotsChecked
    slotsChecked += 0x00100000 ; Ignore reserved slots
    slotsChecked += 0x00200000 ; Ignore reserved slot
    slotsChecked += 0x80000000 ; Ignore FX slot

    int thisSlot = 0x01
    while (thisSlot < 0x80000000)
        if (Math.LogicalAnd(slotsChecked, thisSlot) != thisSlot)
            Armor thisArmor = target.GetWornForm(thisSlot) as Armor
            if (thisArmor)
                wornArmorForms[index] = thisArmor
                index += 1
                slotsChecked += thisArmor.GetSlotMask()
            else
                slotsChecked += thisSlot
            endif
        endif
        thisSlot *= 2
    endWhile
    return wornArmorForms
EndFunction

; ===================================================================
;                               PAGES
; ===================================================================

Function PlayerInformationPage()
    SetCursorFillMode(TOP_TO_BOTTOM)
    ; Page Title and description
    AddHeaderOption(playerInformationPageName)
    AddEmptyOptions(2)

    ; Faction Disguise Values
    AddHeaderOption("$TFS_Faction_Disguise_Status")

    Actor player = Game.GetPlayer()
    Faction[] playerFactions = GetFactionsForActor(player)
    ; For the race bonus value, store the values
    float raceBonusValue = 0

    if playerFactions.Length > 0
        int index = 0
        while index < playerFactions.Length
            Faction currentFaction = playerFactions[index]
            float tempRace = GetRaceBonusValueForFaction(currentFaction)
            if tempRace > raceBonusValue
                raceBonusValue = tempRace
            else
                raceBonusValue = raceBonusValue
            endif

            string editorID = GetFactionEditorID(currentFaction)
            float baseValue = GetDisguiseValueForFaction(currentFaction)
            float bonusValue = GetDisguiseBonusValueForFaction(currentFaction)
            AddTextOption("Faction: " + editorID, Math.Floor(baseValue) + " (Bonus: " + Math.Floor(bonusValue) + ")")
            index += 1
        endWhile
    else
        AddTextOption("$TFS_No_Disguise_Factions_Active", 1)
    endif

    AddHeaderOption("$TFS_Race_Bonus")
    Race playerRace = player.GetRace()
    AddTextOption("Race: " + playerRace.GetName(), Math.Floor(raceBonusValue))
EndFunction

Function FactionManagementPage()
    SetCursorFillMode(TOP_TO_BOTTOM)

    AddHeaderOption(factionManagementPageName)

    int index = 0
    
    AddEmptyOption()
    
    Faction selectedFaction = availableFactions[selectedFactionIndex]
    string factionEditorID = GetFactionEditorID(selectedFaction)

    AddEmptyOption()
    AddHeaderOption("$TFS_Selected_Faction")
    if selectedFaction
        AddTextOption("Faction: " + selectedFaction.GetName(), "", 1)
    else
        AddTextOption("$TFS_No_Faction_Selected", "", 1)
    endif
    

    ; Button to add the selected keyword to the armor
    AddEmptyOption()
    int flag = 0
    if !selectedFaction
        flag = 1
    endif
    _addFactionOptionOID = AddTextOption("$TFS_Add_Faction_Disguise", "", flag)
    AddEmptyOption()

    int factionCount = availableFactions.Length

    ; Switch to the right row
    SetCursorPosition(1)

    AddHeaderOption("$TFS_Selected_Faction")
    AddEmptyOptions(1)
    if factionCount > 0
        ; List all available factions
        index = 0
        while index < factionCount
            Faction currentFaction = availableFactions[index]
            ; factionEditorID = GetFactionEditorID(currentFaction)

            ; Display the faction name and store its index for future selection, factionEditorID
            _availableFactionsMenuOIDs[index] = AddTextOption(currentFaction.GetName(), "", 0)
            index += 1
        endWhile
    else
        AddTextOption("$TFS_No_Factions_Available", "", 1)
    endif
endFunction

Function DisguiseKeywordAssignmentsPage()
    SetCursorFillMode(TOP_TO_BOTTOM)
    AddHeaderOption(factionOverviewPageName)
    if assignedFactionsManage.Length == 0 || assignedKeywordsManage.Length == 0
        Debug.Notification("Trying to reinitialize Keyword Assignments...")
        InitAssignedKeywordFactionPair()
    endif
    
    int index = 0
    ; assignedKeywordManage and assignedFactionManage should have the same length!
    while index < assignedKeywordsManage.Length
        string currentKeyword = assignedKeywordsManage[index]
        Faction currentFaction = assignedFactionsManage[index]
        
        _factionKeywordAssignementsOIDs[index] = AddTextOption(currentKeyword, currentFaction.GetName())

        index += 1
    endWhile

    AddEmptyOptions(2)

    int flag = 1
    if selectedKeywordFactionIndex >= 0
        flag = 0
    endif

    _removeFactionKeywordAssignementOID = AddTextOption("$TFS_Remove_Keyword_Faction", "", flag)
endFunction

Function ArmorKeywordPage()
    SetCursorFillMode(TOP_TO_BOTTOM)
    ; Page Title and description
    AddHeaderOption(armorKeywordSettingPageName, 1)
    AddEmptyOptions(2)

    int index = 0
    while index < wornArmorCount
        if wornArmors[index]
            _wornArmorMenuOIDs[index] = AddTextOption("Worn: " + wornArmors[index].GetName(), wornArmors[index].GetFormID(), 1)
        endif
        index += 1
    endWhile
    AddEmptyOption()

    if selectedArmorIndex != -1 && wornArmors[selectedArmorIndex]
        Armor selectedArmor = wornArmors[selectedArmorIndex]

        AddEmptyOption()
        AddHeaderOption("$TFS_Selected_Armor_Keywords", 0)
        AddEmptyOption()

        Keyword[] keywords = GetArmorKeywords(selectedArmor)
        if keywords.Length > 0
            index = 0
            while index < keywords.Length
                AddTextOption("Keyword: " + keywords[index].GetString(), keywords[index].GetFormID(), 0)
                index += 1
            endWhile
        else
            AddTextOption("$TFS_No_Keywords_Found", "", 1)
        endif

        SetCursorPosition(1)

        AddHeaderOption("$TFS_Add_Keyword_To_Armor", 1)
        _keywordDropdownOID = AddMenuOption("Select Keyword", availableKeywordNames[selectedKeywordIndex])
        AddEmptyOption()

        _addKeywordTextOptionOID = AddTextOption("$TFS_Add_Selected_Keyword", "", 1)

        AddEmptyOption()
        _removeKeywordTextOptionOID = AddTextOption("Remove Selected Keyword", "", 1)
    else
        AddEmptyOption()
        AddHeaderOption("No Armor Selected", 1)
    endif
EndFunction

Function SettingsPage()
    SetCursorFillMode(TOP_TO_BOTTOM)

    AddHeaderOption("$TFS_General_Settings")

    _timeSliderOID = AddSliderOption("$TFS_Time_Threshold", timeToLoseDetection, "$TFS_After_Hours", 0)
    _detectionThresholdSliderOID = AddSliderOption("$TFS_Detection_Threshold", detectionThreshold, "{0}%", 0)
    _detectionRadiusOID = AddSliderOption("Detection Radius", detectionRadius, "{0} Units")
    AddEmptyOption()
    _useFOVOptionOID = AddToggleOption("Use FOV Check?", useFOVCheck, 0)
    _useLOSOptionOID = AddToggleOption("Use Line-Of-Sight Check?", useLOSCheck, 0)

    AddEmptyOptions(2)

    AddHeaderOption("$TFS_Misc")
    ; Reload the factions, because of modded factions, if the user changes the load order, the FormID changes!
    _resetModTextOptionOID = AddTextOption("$TFS_Reset_Mod", "", 1) ; Not implemented!
EndFunction

; ===================================================================
;                              Utils
; ===================================================================

Function AddEmptyOptions(int amount)
    int index = 0
    while index < amount
        AddEmptyOption()
        index += 1
    EndWhile
EndFunction

Function HandleArmorSelection(int a_option)
    int index = 0
    bool breakLoop = false
    while !breakLoop && index < wornArmorCount
        if a_option == _wornArmorMenuOIDs[index] && wornArmors[index]
            selectedArmorIndex = index
            ForcePageReset()
            breakLoop = true
        endif
        index += 1
    endWhile
endFunction

Function HandleKeywordFactionSelection(int a_option)
    int index = 0
    bool breakLoop = false
    while !breakLoop && index < _factionKeywordAssignementsOIDs.Length
        if a_option == _factionKeywordAssignementsOIDs[index]
            selectedKeywordFactionIndex = index
            ForcePageReset()
            breakLoop = true
        endif
        index += 1
    endWhile
endFunction

Function HandleFactionSelection(int a_option)
    int index = 0
    bool breakLoop = false
    while !breakLoop && index < availableFactions.Length
        if a_option == _availableFactionsMenuOIDs[index] && availableFactions[index]
            Debug.Notification("Selected Faction: " + availableFactions[index].GetName())
            selectedFactionIndex = index
            ForcePageReset()
            breakLoop = true
        endif
        index += 1
    endWhile
endFunction

Function HandleAddKeywordToArmor()
    Keyword newKeyword = Game.GetFormFromFile(availableKeywordFormIDs[selectedKeywordIndex], "TrueFactionSystem.esp") as Keyword
    Debug.Notification(""+availableKeywordFormIDs[selectedKeywordIndex])
    Debug.Notification(""+wornArmors[selectedArmorIndex].GetName())
    if AddKeywordToArmor(wornArmors[selectedArmorIndex], newKeyword)
        Debug.Notification("Keyword successfully added to: " + wornArmors[selectedArmorIndex].GetName())
    else
        Debug.Notification("Failed to add keyword.")
    endif
endFunction

Function HandleRemoveKeywordFromArmor()
    Keyword newKeyword = Game.GetFormFromFile(availableKeywordFormIDs[selectedKeywordIndex], "TrueFactionSystem.esp") as Keyword
    Debug.Notification(""+availableKeywordFormIDs[selectedKeywordIndex])
    if RemoveKeywordFromArmor(wornArmors[selectedArmorIndex], newKeyword)
        Debug.Notification("Keyword successfully removed from: " + wornArmors[selectedArmorIndex].GetName())
    else
        Debug.Notification("Failed to remove keyword.")
    endif
endFunction

Function HandleAddFactionAsKeyword()
    Faction selectedFaction = availableFactions[selectedFactionIndex]
    if selectedFaction
        Keyword newKeyword = HandleAddFactionFromMCM(selectedFaction)  ; Call native function to add the faction
        if newKeyword
            string factionEditorID = GetFactionEditorID(selectedFaction)
            AddNewFaction(selectedFaction.GetName(), newKeyword)
        else
            Debug.Notification("Failed to add faction: " + selectedFaction.GetName())
        endif
    endif
endFunction

Function HandleRemoveKeywordFactionAssignements()
    if RemoveFactionKeywordAssignment(assignedKeywordsManage[selectedKeywordFactionIndex], assignedFactionsManage[selectedKeywordFactionIndex])
        Keyword factionKeyword = GetKeywordByEditorID(assignedKeywordsManage[selectedKeywordFactionIndex])
        RemoveNewFaction(assignedFactionsManage[selectedKeywordFactionIndex].GetName(), factionKeyword)
        InitAssignedKeywordFactionPair()
        selectedKeywordFactionIndex = -1
        Debug.Notification("Removed keyword: " + assignedKeywordsManage[selectedKeywordFactionIndex] + " from faction: " + assignedFactionsManage[selectedKeywordFactionIndex].GetName())
    else
        Debug.Notification("Failed to remove Keyword-Faction assignement!")
    endif
endFunction

; ===================================================================
;                       Implements SKI Functions
; ===================================================================

Event OnOptionSliderOpen(int option)
    if option == _timeSliderOID
        SetSliderDialogStartValue(timeToLoseDetection)
        SetSliderDialogDefaultValue(2.0)
        SetSliderDialogRange(1.0, 168.0)
        SetSliderDialogInterval(1.0)
    elseif option == _detectionThresholdSliderOID
        SetSliderDialogStartValue(detectionThreshold)
        SetSliderDialogDefaultValue(61.0)
        SetSliderDialogRange(0.0, 100.0)
        SetSliderDialogInterval(1.0)
    elseif option == _detectionRadiusOID
        SetSliderDialogStartValue(detectionRadius)
        SetSliderDialogDefaultValue(400.0)
        SetSliderDialogRange(50.0, 2000.0)
        SetSliderDialogInterval(1.0)
    endif
EndEvent

Event OnOptionSliderAccept(int sliderID, float newValue)
    if sliderID == _timeSliderOID
        timeToLoseDetection = newValue
        SetTimeToLoseDetection(timeToLoseDetection)
        SetSliderOptionValue(_timeSliderOID, timeToLoseDetection, "$TFS_After_Hours")
    elseif sliderID == _detectionThresholdSliderOID
        detectionThreshold = newValue
        SetDetectionThreshold((newValue / 100.0) as float)
        SetSliderOptionValue(_detectionThresholdSliderOID, detectionThreshold, "{0}%")
    endif
EndEvent

Event OnPageApply(String pageName)
    if pageName == modSettingsPageName
        SetTimeToLoseDetection(timeToLoseDetection)
        SetDetectionThreshold(detectionThreshold)
    endif
EndEvent

; This event handles resetting and updating the page
Event OnPageReset(string a_page)
    if a_page == ""
        LoadCustomContent("skyui/TrueFactionSystem/TFS.dds", 120, -33)
        return
    else
        UnloadCustomContent()
    endif

    if (a_page == playerInformationPageName)
        PlayerInformationPage()
    elseif (a_page == armorKeywordSettingPageName)
        InitWornArmor()
        ArmorKeywordPage()
    elseif (a_page == factionManagementPageName)
        FactionManagementPage()
    elseif (a_page == factionOverviewPageName)
        InitAssignedKeywordFactionPair()
        DisguiseKeywordAssignmentsPage()
    elseif (a_page == modSettingsPageName)
        SettingsPage()
    endif
EndEvent

Event OnOptionMenuOpen(int a_option)
    if a_option == _keywordDropdownOID
        ; Populate the menu with available keywords (factions)
        SetMenuDialogOptions(availableKeywordNames)
        SetMenuDialogStartIndex(selectedKeywordIndex)
        SetMenuDialogDefaultIndex(0)
    endif
EndEvent

Event OnOptionSelect(int a_option)
    HandleFactionSelection(a_option)
    HandleArmorSelection(a_option)
    HandleKeywordFactionSelection(a_option)
    

    if a_option == _addKeywordTextOptionOID && selectedArmorIndex >= 0 && wornArmors[selectedArmorIndex]
        HandleAddKeywordToArmor()
    elseif a_option == _removeKeywordTextOptionOID && selectedArmorIndex >= 0 && wornArmors[selectedArmorIndex]
        HandleRemoveKeywordFromArmor()
    endif
    
    if a_option == _addFactionOptionOID && selectedFactionIndex >= 0 && selectedFactionIndex < availableFactions.Length && availableFactions[selectedFactionIndex]
        HandleAddFactionAsKeyword()
    endif

    if a_option == _removeFactionKeywordAssignementOID && selectedKeywordFactionIndex >= 0
        HandleRemoveKeywordFactionAssignements()
    endif

    if a_option == _useFOVOptionOID
        useFOVCheck = !useFOVCheck
        SetToggleOptionValue(_useFOVOptionOID, useFOVCheck)
        SetUseFOVCheck(useFOVCheck)
    endif
    if a_option == _useLOSOptionOID
        useLOSCheck = !useLOSCheck
        SetToggleOptionValue(_useLOSOptionOID, useLOSCheck)
        SetUseLineOfSightCheck(useLOSCheck)
    endif

    ForcePageReset()
EndEvent

; This event handles the selection from the keyword dropdown
Event OnOptionMenuAccept(int a_option, int a_index)
    if a_option == _keywordDropdownOID
        selectedKeywordIndex = a_index
        Debug.Notification("Selected keyword: " + availableKeywordNames[selectedKeywordIndex])
        ; Refresh the menu to show the updated keyword selection
        ForcePageReset()
    endif
EndEvent

Event OnOptionHighlight(int a_option)
    if a_option == _timeSliderOID
        SetInfoText("$TFS_Time_Threshold_Info")
    elseif a_option == _detectionThresholdSliderOID
        SetInfoText("$TFS_Detection_Threshold_Info")
    elseif a_option == _useFOVOptionOID
        SetInfoText("Decide if NPCs should use a custom FOV function to detect the player.")
    elseif a_option == _useLOSOptionOID
        SetInfoText("Decide if NPCs need the Player to be in their line of sight (independet from FOV).")
    else
        SetInfoText("")
    endif
EndEvent