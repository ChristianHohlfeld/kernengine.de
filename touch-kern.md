# touch-kern (kern true)
started: {"body":"sc-touch-on on-foot-mode mobile-gameplay-compact scui-playing solo-flight edge-hud thumb-weapon","mode":"foot","weapon":"WEAPON · GLOCK"}
```
#thumbWeapon                 608,320 60x60 z23 ⇄GLOCK
#footJump                    536,316 64x64 z22 JUMP
#mobileGameplayMode          176,340 96x40 z40 DRONE
#airStrikeButton             10,156 58x58 z22 ✈STRIKE
#handGrenadeBtn              76,150 64x64 z23 ●GRANATE3
#scopeButton                 -
#footMove                    0,222 168x168 z20 MOVE
#footLook                    676,222 168x168 z20 LOOK
#musicSwitch                 668,9 40x36 z100001 
#soundSwitch                 716,9 40x36 z100001 
#proximityVoiceBar           138,8 81x36 z100002 🎤🔈MIC AUS
#mobileGameplayReset         764,9 72x36 z47 RESET
#gpsLiveButton               591,9 69x36 z100001 📍 GPS
#worldLookHud                736,52 100x100 z6 MINIMAP · N↑ · TOPSNAP⛶N
#gameExitButton              8,8 56x36 z100003 EXIT
#mobileGameplayMultiplayer   70,8 60x36 z100003 MULTI
#mobileGameplayDock          369,5 106x46 z46 WEAPON · GLOCKSETTINGS
.phone-settings-button       -
```
overlaps: none
visible controls:
```
button#-.- 798,61 27x27 "⛶"
button#footJump.- 536,316 64x64 "JUMP"
button#proximityVoicePtt.- 138,8 38x36 "🎤"
button#proximityVoiceMute.- 181,8 38x36 "🔈"
button#gameExitButton.- 8,8 56x36 "EXIT"
button#mobileGameplaySettings.- 374,10 96x36 "SETTINGS"
button#mobileGameplayMode.hud-mode-switch 176,340 96x40 "DRONE"
button#mobileGameplayMultiplayer.hud-after-exit 70,8 60x36 "MULTI"
button#mobileGameplayReset.- 764,9 72x36 "RESET"
button#airStrikeButton.- 10,156 58x58 "✈STRIKE"
button#handGrenadeBtn.show 76,150 64x64 "●GRANATE3"
svg#-.- 76,150 64x64 ""
button#thumbWeapon.show 608,320 60x60 "⇄GLOCK"
button#soundSwitch.- 716,9 40x36 "Effects on/off"
svg#-.- 726,17 20x20 ""
button#musicSwitch.- 668,9 40x36 "Music on/off"
svg#-.- 678,17 20x20 ""
button#gpsLiveButton.- 591,9 69x36 "📍 GPS"
```
weapon tap: WEAPON · GLOCK -> WEAPON · SNIPER
drone tap: after 0.4 s drone · after 2.4 s drone
back on foot: foot
weapon now WEAPON · SNIPER
scope button {"x":608,"y":254,"w":60,"h":60,"z":"23","text":"◎SCOPE"}
scoped: false · scope button now {"x":608,"y":254,"w":60,"h":60,"z":"23","text":"◎SCOPE"}
after second tap scoped: false
