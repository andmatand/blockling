''Blockman: The FreeBASIC Remake of the Old QBASIC version I made
''Andrew Anderson
''Started January 2006
''
''===TO DO===
''-When a player starts to climb on top of a block, but another player immediately pushes that block, he
'' doesn't climb all the way on it
''-Undo can sometimes undo too far back, to a point that doesn't preceed the one after it
'' 
''
''
''
''===IDEAS FOR LEVELS===
''If you give me a ??? I'll give you this [key I found on the ground, block I found]
''sandwich
''screwdriver
''fishy
''hamburger
''birthday cake
''homing misile
''kitty
''bunny
''turtle
''ruby
''
''I want to jump off here, but I am too scared to do it myself.  If you promise to push me off the edge, I will give you this key.
''
''Oh no!  I forgot!  What is the capital of Asyria again?!
''
''Lost children
''
''I was just taking an afternoon stroll here in the morgue, but now I can't find my way out!  Can you help me?
''
''
''===IDEAS FOR ITEMS===
''Spinach - makes player strong to push/lift multiple blocks
''Glue - makes player able to pull blocks
''Bad Guys - They try to come toward you and capture you
''Turrets - Can shoot you from far away, but only in a straight line
''
''===BLOCKS===
''Breakable block - can only be moved 3 times and gets more cracked each time until it shatters
''



'declare sub DrawString (x as integer, y as integer, w as integer, s as string, zoomX as integer, zoomY as integer)
declare sub DrawBlocks
declare sub EraseBlocks
declare sub Blocks
declare sub DeleteTempFolder
declare sub DrawBricks
declare function BoxOverlap (byval x1 as integer, byval y1 as integer, byval w1 as integer, byval h1 as integer, byval x2 as integer, byval y2 as integer, byval w2 as integer, byval h2 as integer) as byte
declare sub Players
declare sub DrawPlayers
declare function BoxClear (byval x as integer, byval y as integer, byval w as integer, byval h as integer) as byte
declare function BlockNumber (x as integer, y as integer, w as integer, h as integer) as integer
declare function BrickNumber (x as integer, y as integer, w as integer, h as integer) as integer
declare function PlayerNumber (byval x as integer, byval y as integer) as byte
declare sub SaveUndo
declare sub LoadUndo
declare sub AdvancePaths(byval x as integer, byval y as integer, byref x3 as integer, byref y3 as integer, byref XMoving as byte, byref YMoving as byte, byref path as string)
declare sub CheckForStep (byval a as integer)
'declare sub MoveBlocksOnTop (byval a as integer)
declare sub Physics (byval axis as byte, byref x as integer, byref y as integer, byref w as integer, byref h as integer, byref xSpeed as integer, byref YSpeed as integer, byref XGravity as integer, byref YGravity as integer, byref path as string, byref XMoving as byte, byref YMoving as byte, byref x2 as integer, byref y2 as integer, byref x3 as integer, byref y3 as integer, byref blockNum as integer)
declare sub QuitGame
declare function RoomAboveBlock(byval a as integer, byval p as integer, byval recursive as byte) as byte
declare function RoomLeftBlock(byval a as integer, byval p as integer) as byte
declare function RoomRightBlock(byval a as integer, byval p as integer) as byte
'declare function RoomBelowBlock(byval a as integer, byval p as integer) as byte
declare sub DrawTorches

#include "buttons.bi"

randomize timer

dim shared as byte tileW, tileH
tileW = 16
tileH = 16
dim shared as integer screenW, screenH
screenW = 640
screenH = 480

dim shared as byte numPlayers
numPlayers = 2

dim shared as byte numKeys
numKeys = 5
dim shared KeyOn(numPlayers - 1, numKeys - 1) as byte
dim shared KeyTimer(numPlayers - 1, numKeys - 1) as double
dim shared playerKey(numPlayers - 1, numKeys - 1) as integer
playerKey(0, 0) = SC_LEFT
playerKey(0, 1) = SC_RIGHT
playerKey(0, 2) = SC_UP
playerKey(0, 3) = SC_DOWN
playerKey(0, 4) = SC_ENTER
playerKey(1, 0) = SC_A
playerKey(1, 1) = SC_D
playerKey(1, 2) = SC_W
playerKey(1, 3) = SC_S
playerKey(1, 4) = SC_LSHIFT


dim shared as integer maxBlocks, numBlocks
maxBlocks = 400
numBlocks = 100
dim shared as integer blockX(maxBlocks - 1), blockY(maxBlocks - 1), blockW(maxBlocks - 1), blockH(maxBlocks - 1)
dim shared as byte blockMouth(maxBlocks - 1)
dim shared as integer blockX2(maxBlocks - 1), blockY2(maxBlocks - 1)
dim shared as integer blockX3(maxBlocks - 1), blockY3(maxBlocks - 1)
dim shared as byte blockXMoving(maxBlocks - 1), blockYMoving(maxBlocks - 1)
dim shared as integer blockXSpeed(maxBlocks - 1), blockYSpeed(maxBlocks - 1)
dim shared as integer blockXGravity, blockYGravity
dim shared as byte blockDir(maxBlocks - 1)
dim shared as string blockPath(maxBlocks - 1)
dim shared as byte blockType(maxBlocks - 1), blockFalling(maxBlocks - 1)
dim shared as byte blockStrong(maxBlocks - 1), blockDidGravity(maxBlocks - 1)
dim shared as integer playerMovingBlock(maxBlocks - 1)
dim shared playerBlink(maxBlocks - 1) as double

dim shared numBricks as integer
numBricks = 200
dim shared as integer brickX(numBricks - 1), brickY(numBricks - 1), brickW(numBricks - 1), brickH(numBricks - 1)

dim shared as integer maxTorches = 100, numTorches
dim shared as integer torchX(maxTorches - 1), torchY(maxTorches - 1)
dim shared as byte torchFlame(maxTorches - 1)

dim shared moveTimer as double
dim shared gameSpeed as double
dim shared moveGame as byte

dim shared BMPpath as string
BMPpath = "bmp\"
dim shared undoFile as string
undoFile = "temp\undo"
dim shared as integer undoNumber, undoStart
dim shared as integer maxUndoLevels = 10
dim undoTimer as double


screenres 640, 480, 16, 2
screenset 1, 0
cls
SETMOUSE , , 0

dim shared as integer mouseX, mouseY
dim shared as string key

dim as integer a, b, ok
a = 4 + (tileW * tileH * 2)

dim shared blockErase(maxBlocks - 1, a) as integer

dim shared block0(a) as integer
dim shared block1(a) as integer
dim shared brick0(a) as integer
bload BMPpath + "block0.bmp", VARPTR(block0(0))
bload BMPpath + "block1.bmp", VARPTR(block1(0))
bload BMPpath + "brick0.bmp", VARPTR(brick0(0))

dim shared aelio0_0(a) as integer
dim shared aelio0_1(a) as integer
dim shared aelio0_2(a) as integer
dim shared aelio0_3(a) as integer
dim shared aelio0_4(a) as integer
dim shared aelio1_0(a) as integer
dim shared aelio1_1(a) as integer
dim shared aelio1_2(a) as integer
dim shared aelio1_3(a) as integer
dim shared aelio1_4(a) as integer
dim shared aelio2_0(a) as integer
dim shared aelio2_1(a) as integer
dim shared aelio2_2(a) as integer
dim shared aelio2_3(a) as integer
dim shared aelio2_4(a) as integer
bload BMPpath + "aelio0_0.bmp", VARPTR(aelio0_0(0))
bload BMPpath + "aelio0_1.bmp", VARPTR(aelio0_1(0))
bload BMPpath + "aelio0_2.bmp", VARPTR(aelio0_2(0))
bload BMPpath + "aelio0_3.bmp", VARPTR(aelio0_3(0))
bload BMPpath + "aelio0_4.bmp", VARPTR(aelio0_4(0))
bload BMPpath + "aelio1_0.bmp", VARPTR(aelio1_0(0))
bload BMPpath + "aelio1_1.bmp", VARPTR(aelio1_1(0))
bload BMPpath + "aelio1_2.bmp", VARPTR(aelio1_2(0))
bload BMPpath + "aelio1_3.bmp", VARPTR(aelio1_3(0))
bload BMPpath + "aelio1_4.bmp", VARPTR(aelio1_4(0))
bload BMPpath + "aelio2_0.bmp", VARPTR(aelio2_0(0))
bload BMPpath + "aelio2_1.bmp", VARPTR(aelio2_1(0))
bload BMPpath + "aelio2_2.bmp", VARPTR(aelio2_2(0))
bload BMPpath + "aelio2_3.bmp", VARPTR(aelio2_3(0))
bload BMPpath + "aelio2_4.bmp", VARPTR(aelio2_4(0))

dim shared torch0_0(a) as integer
dim shared torch0_1(a) as integer
dim shared torch0_2(a) as integer
dim shared torch0_3(a) as integer
dim shared torch0_4(a) as integer
dim shared torch0_5(a) as integer
dim shared torch0_6(a) as integer
dim shared torch0_7(a) as integer
bload BMPpath + "torch0_0.bmp", VARPTR(torch0_0(0))
bload BMPpath + "torch0_1.bmp", VARPTR(torch0_1(0))
bload BMPpath + "torch0_2.bmp", VARPTR(torch0_2(0))
bload BMPpath + "torch0_3.bmp", VARPTR(torch0_3(0))
bload BMPpath + "torch0_4.bmp", VARPTR(torch0_4(0))
bload BMPpath + "torch0_5.bmp", VARPTR(torch0_5(0))
bload BMPpath + "torch0_6.bmp", VARPTR(torch0_6(0))
bload BMPpath + "torch0_7.bmp", VARPTR(torch0_7(0))

'get (0, 0)-(tileW - 1, tileH - 1), zombie0(0)
'cls
'line (0,0)-(639, 479), rgb(255,0,0), bf
'put (100, 100), zombie0(0), trans
'sleep : end

restart:
DeleteTempFolder
mkdir "temp"
undoNumber = -1
undoStart = -1
for a = numPlayers to numBlocks - 1
    do
        blockX(a) = (int(rnd * 29) + 2) * tileW
        blockY(a) = (int(rnd * (27 - 3)) + 3) * tileH
        ok = 1
        for b = 0 to numBlocks - 1
            if b <> a then
                if blockX(a) = blockX(b) and blockY(a) = blockY(b) then
                    ok = 0
                    exit for
                end if
            end if
        next b
    loop until ok = 1
    blockMouth(a) = int(rnd * 2)
next a

''player 1
blockX(0) = tileW * 10
blockY(0) = tileH * 2

''player 2
blockX(1) = tileW * 9
blockY(1) = tileH * 2


''Torches
numTorches = 3
torchX(0) = 1 * tileW
torchY(0) = 19 * tileH

torchX(1) = 17 * tileW
torchY(1) = 1 * tileH

torchX(2) = 18 * tileW
torchY(2) = 1 * tileH


for a = 0 to 31
    brickX(a) = (a + 2) * tileW
    brickY(a) = (tileH * 28)
next a

for a = 32 to 63
    brickX(a) = ((a - 32) + 2) * tileW
    brickY(a) = 16
next a

for a = 64 to 91
    brickX(a) = 33 * tileW
    brickY(a) = (tileH * 28) - ((a - 64) * tileH)
next a

for a = 92 to 119
    brickX(a) = 1 * tileW
    brickY(a) = (tileH * 28) - ((a - 92) * tileH)
next a

brickX(120) = 5 * tileW
brickY(120) = 26 * tileH

'blockX(1) = 6 * tileW ''player 2
'blockY(1) = 27 * tileH
'
'blockX(0) = 8 * tileW ''player 1
'blockY(0) = 25 * tileH
'
'blockX(2) = 5 * tileW
'blockY(2) = 25 * tileW
'
'blockX(3) = 6 * tileW
'blockY(3) = 25 * tileW
'
'blockX(4) = 6 * tileW
'blockY(4) = 26 * tileW
'
'blockX(6) = 10 * tileW
'blockY(6) = 26 * tileW
'
'''bottom
'blockX(7) = 5 * tileW
'blockY(7) = 27 * tileW
'
'blockX(9) = 7 * tileW
'blockY(9) = 27 * tileW
'
'blockX(10) = 8 * tileW
'blockY(10) = 27 * tileW
'
'blockX(11) = 9 * tileW
'blockY(11) = 27 * tileW
'
'blockX(8) = 10 * tileW
'blockY(8) = 27 * tileW
'
'blockX(5) = 11 * tileW
'blockY(5) = 26 * tileW
'

for a = 121 to numBricks - 1
    do
        brickX(a) = (int(rnd * 29) + 2) * tileW
        brickY(a) = (int(rnd * (27 - 3)) + 3) * tileH
        ok = 1
        for b = 0 to numBricks - 1
            if b <> a then
                if brickX(a) = brickX(b) and brickY(a) = brickY(b) then
                    ok = 0
                    exit for
                end if
            end if
        next b
        for b = 0 to numBlocks - 1
            if brickX(a) = blockX(b) and brickY(a) = blockY(b) then
                ok = 0
                exit for
            end if
        next b
    loop until ok = 1
next a


gameSpeed = .005
blockXGravity = 0
blockYGravity = 1'tileH / 2
for a = 0 to numBricks - 1
    brickW(a) = tileW
    brickH(a) = tileH
next a
for a = 0 to numBlocks - 1
    blockW(a) = tileW
    blockH(a) = tileH
    blockXMoving(a) = 0
    blockYMoving(a) = 0
    blockXSpeed(a) = 1'blockW(a) / 2
    blockYSpeed(a) = 1'blockW(a) / 2
    blockDir(a) = 1
    blockType(a) = 0
    blockStrong(a) = 0
next a

for a = 0 to numPlayers - 1
    playerMovingBlock(a) = -1
    blockType(a) = 1
next a
blockStrong(0) = 1 ''spinach

cls
bload bmpPath + "back.bmp"
'paint (639, 0), rgb(0, 255, 0)
'DrawBricks
do
    
    GetMouse mouseX, mouseY
    key = inkey
    
    Blocks
    
    'moveGame = 0
    'if timer >= moveTimer + gameSpeed or timer < moveTimer then moveGame = 1
    moveGame = 1
    
    'screenlock
    'line (0, 0)-(screenW - 1, screenH - 1), rgb(0, 0, 0), bf
    'for a = 0 to 500
    '    circle(int(rnd * screenW), int(rnd * screenH)), int(rnd* 100), rgb(0,0,0)'rgb(int(rnd*256),int(rnd*256),int(rnd*256))
    'next a
    locate 1, 55 : print timer; "   "
    DrawBricks
    DrawTorches
    DrawBlocks    
    'screenunlock
    
    locate 1, 1 : print undoNumber; "   "
    locate 2, 1 : print undoStart; "   "
    
    pcopy 1, 0
    sleep 7

    EraseBlocks
    

    'if moveGame = 1 then moveTimer = timer
    'SCREENSYNC
    
    if multikey(sc_escape) then QuitGame
    if multikey(sc_r) then goto restart
    if timer >= undoTimer + .25 then
        if multikey(sc_U) then
            LoadUndo
            undoTimer = timer
        end if
    end if
    while inkey$ <> "": wend

loop' until inkey$ = CHR$(27)
'end





sub AdvancePaths(byval x as integer, byval y as integer, byref x3 as integer, byref y3 as integer, byref XMoving as byte, byref YMoving as byte, byref path as string)
dim as string s
dim as integer n, b

if XMoving = 0 and YMoving = 0 and len(path) > 0 then
    for b = 1 to len(path)
        s = mid(path, b, 1)
        if s = "x" or s = "y" then
            n = val(left(path, b - 1))
            if s = "x" then XMoving = n else YMoving = n
            x3 = x
            y3 = y
            path = right(path, len(path) - b)
            exit for
        end if
    next b
end if
end sub







sub Blocks
dim as integer a, b, m, d, y2
dim as byte ok, didUndo

if MoveGame <> 1 then exit sub
'for a = 0 to numBlocks - 1
'    blockX2(a) = blockX(a)
'    blockY2(a) = blockY(a)
'next a


'a = 0
'locate 1,11 : print BoxClear(blockX(a), blockY(a) + blockH(a), blockW(a), 1)
didUndo = 0

for a = 0 to numBlocks - 1
    if blockType(a) = 1 then
        'b = BlockNumber(blockX(a) + (blockW(a) / 2), blockY(a) - 1)
        
        for b = 0 to numKeys - 1
            m = multikey(playerKey(a, b))
            if not m then keyOn(a, b) = 0
            if m then
                if KeyOn(a, b) <> 0 then 
                    if timer >= KeyTimer(a, b) + .25 or timer < KeyTimer(a, b) then keyOn(a, b) = 1 else keyOn(a, b) = -1
                end if
                if KeyOn(a, b) = 0 then
                    KeyTimer(a, b) = timer
                    KeyOn(a, b) = 1
                end if
            end if
        next b
    
        b = playerMovingBlock(a)
        if b <> -1 then
            if (blockXMoving(b) = 0 and blockYMoving(b) = 0 and blockPath(b) = "" and BoxClear(blockX(b), blockY(b) + blockH(b), blockW(b), 1) = 0) or blockY(b) > blockY(a) then playerMovingBlock(a) = -1
            'if blockXMoving(b) = 0 and blockYMoving(b) = 0 and blockPath(b) = "" then playerMovingBlock(a) = -1' else cantmove = 1
        end if
        
       
        

        ''Check if player is standing on ground
        ok = 1
        if blockYGravity > 0 then
            if BoxClear(blockX(a), blockY(a) + blockH(a), blockW(a), 1) = 1 then
                ok = 0
            else
                if blockFalling(a) = 1 then blockFalling(a) = 2
            end if
        end if

        if blockXMoving(a) = 0 and blockYMoving(a) = 0 and blockPath(a) = "" and ok = 1 then
            
            ''Push Block
            if keyOn(a, 4) > 0 and playerMovingBlock(a) = -1 then
                b = BlockNumber(blockX(a) + (blockDir(a) * blockW(a)), blockY(a) + (blockH(a) - 1) - (blockY(a) mod tileH), 1, 1)
                'b = BlockNumber(blockX(a) + (blockDir(a) * blockW(a)), blockY(a) + (blockH(a) - 1), 1, 1)
                'if blockDir(a) = 1 then b = BlockNumber(blockX(a) + (blockDir(a) * blockW(a)), blockY(a) + (blockH(a) - 1) - (blockY(a) mod tileH), 1, 1)
                'if blockDir(a) = -1 then b = BlockNumber(blockX(a) - 1, blockY(a) + (blockH(a) - 1) - (blockY(a) mod tileH), 1, 1)
                if b <> -1 then
                    if didUndo = 0 then
                        SaveUndo
                        didUndo = 1
                    end if
                    'blockXMoving(b) = playerDir(a)
                    'blockX3(b) = blockX(b)
                    playerMovingBlock(a) = b
                    blockPath(b) = str(blockW(b) * blockDir(a)) + "x"
                    if blockStrong(a) = 1 and blockStrong(b) = 0 then blockStrong(b) = 2
                    'goto pushedButton
                end if
            end if
            
            ''Go left
            if keyOn(a, 0) > 0 and keyOn(a, 1) = 0 then
                if blockDir(a) = -1 and playerMovingBlock(a) = -1 then
                    if didUndo = 0 then
                        SaveUndo
                        didUndo = 1
                    end if
                    'playerXMoving(a) = -playerW(a)
                    'playerX3(a) = playerX(a)
                    blockPath(a) = str(blockW(a) * blockDir(a)) + "x"
                    CheckForStep(a)
                else blockDir(a) = -1
                end if
                'goto pushedButton
            end if
            
            ''Go right
            if keyOn(a, 1) > 0 and not keyOn(a, 0) then
                if blockDir(a) = 1 and playerMovingBlock(a) = -1 then
                    if didUndo = 0 then
                        SaveUndo
                        didUndo = 1
                    end if
                    'playerXMoving(a) = playerW(a)
                    'playerX3(a) = playerX(a)
                    blockPath(a) = str(blockW(a) * blockDir(a)) + "x"
                    CheckForStep(a)
                else blockDir(a) = 1
                end if
                'goto pushedButton
            end if
            
            ''set down block
            if keyOn(a, 3) > 0 and playerMovingBlock(a) = -1 then
                b = BlockNumber(blockX(a), blockY(a) - 1, blockW(a), 1)
                if b <> - 1 then
                    if blockXMoving(b) = 0 and blockYMoving(b) = 0 and blockPath(b) = "" then
                        if didUndo = 0 then
                            SaveUndo
                            didUndo = 1
                        end if
                        playerMovingBlock(a) = b
                        blockPath(b) = str(blockDir(a) * blockW(b)) + "x"
                        
                        ''Set Down block on top of block/brick next to player
                        if BoxClear(blockX(b) + (blockW(b) * blockDir(a)), blockY(b), blockW(b), blockH(b)) = 0 then
                            if BoxClear(blockX(b) + (blockW(b) * blockDir(a)), blockY(b) - (blockY(b) mod tileH), blockW(b), blockH(b)) = 1 then
                                'line (blockX(b) + (blockW(b) * blockDir(a)), blockY(b) - (blockY(b) mod tileH))-step(blockW(b) - 1, blockH(b) - 1), rgb(255,0,0), b
                                'print blockY(b) mod tileH
                                blockPath(b) = str(-(blockY(b) mod tileH)) + "y" + blockPath(b)
                                if blockStrong(a) = 1 and blockStrong(b) = 0 then blockStrong(b) = 2
                                'locate 1, 10
                                'print blockPath(b)
                            end if
                        end if
                        'goto pushedButton
                    end if
                end if
            end if
            
            ''pick up block
            if keyOn(a, 2) > 0 and playerMovingBlock(a) = -1 and keyOn(a, 0) = 0 and keyOn(a, 1) = 0 then
                if blockDir(a) = -1 then d = -1 else d = blockW(a)
                b = BlockNumber(blockX(a) + d, blockY(a) + (blockH(a) - 1) - (blockY(a) mod tileH), 1, 1)
                if b <> -1 then
                    if BoxClear(blockX(a), blockY(a) - blockH(b), blockW(b), blockH(b)) then
                        if didUndo = 0 then
                            SaveUndo
                            didUndo = 1
                        end if
                        playerMovingBlock(a) = b
                        'blockPath(b) = str(-(blockH(a) + (blockY(b) mod tileH))) + "y" + str(-blockDir(a)) + "x 2y " + str((blockW(b) - 1) * -blockDir(a)) + "x"
                        blockPath(b) = str(-(blockH(a) + (blockY(b) mod tileH))) + "y" + str(blockW(b) * -blockDir(a)) + "x"
                        if blockStrong(a) = 1 and blockStrong(b) = 0 then blockStrong(b) = 2
                        'blockYMoving(b) = -1
                        'blockY3(b) = blockY(b)
                        'playerBlock(a) = b
                        'goto pushedButton
                    end if
                end if
            end if
        end if
    end if
'pushedButton:
next a



''Player Blocks
'for a = 0 to numBlocks - 1
'    if blockType(a) = 1 then
'        playerBlock(a) = -1
'        b = BlockNumber(blockX(a), blockY(a) - 1, blockW(a), 1)
'        if b <> -1 then
'            if blockPath(b) = "" then'and blockXMoving(b) = 0 and blockYMoving(b) = 0 then
'                if BlockNumber(blockX(b), blockY(b) - 1, blockW(b), 1) = -1 then
'                    'playerBlock(a) = b
'                    'blockPath(b) = blockPath(a)
'                    ''blockX(b) = blockX(a)
'                    ''blockXMoving(b) = blockXMoving(a)
'                    ''blockXSpeed(b) = blockXSpeed(a)
'                end if
'            end if
'        end if
'    end if
'next a
'

''Process Paths
for a = 0 to numBlocks - 1
    blockDidGravity(a) = 0
    AdvancePaths(blockX(a), blockY(a), blockX3(a), blockY3(a), blockXMoving(a), blockYMoving(a), blockPath(a))
next a


''Block Gravity / Collisions
for a = 0 to numBlocks - 1
    if blockDidGravity(a) = 0 then
        Physics(4, blockX(a), blockY(a), blockW(a), blockH(a), blockXSpeed(a), blockYSpeed(a), blockXGravity, blockYGravity, blockPath(a), blockXMoving(a), blockYMoving(a), blockX2(a), y2, blockX3(a), blockY3(a), a)
    end if
next a

''Move Blocks on their paths / Collisions
for a = 0 to numBlocks - 1
   
    y2 = blockY2(a)

    if blockType(a) = 1 then
        ''Make player sink or grow because of holding block or not holding block
        if blockH(a) < tileH then
            if BlockNumber(blockX(a), blockY(a) - 1, blockW(a), 1) = -1 then
                blockH(a) += 1
                blockY(a) -= 1
                y2 -= 1
                if blockFalling(a) = 2 then blockFalling(a) = 0
            end if
        end if
        if blockH(a) > tileH - 2 then
            ''If block is on top of him, or he just fell from a tall height
            if BlockNumber(blockX(a), blockY(a) - 1, blockW(a), 1) <> -1 or blockFalling(a) = 2 then
                blockH(a) -= 1
                blockY(a) += 1
            end if
        else
            if blockFalling(a) = 2 then blockFalling(a) = 0
        end if
        
        if blockFalling(a) = 0 then
            ''If player is falling farther than height * 1.5
            if BoxClear(blockX(a), blockY(a) + blockH(a), blockW(a), blockH(a) * 1.5) = 1 then blockFalling(a) = 1
        end if
        
        'if blockFalling(a) = 2 then ''If player just hit the ground
        '    blockFalling(a) = 0
        'end if

    end if
    
    Physics(-3, blockX(a), blockY(a), blockW(a), blockH(a), blockXSpeed(a), blockYSpeed(a), blockXGravity, blockYGravity, blockPath(a), blockXMoving(a), blockYMoving(a), blockX2(a), y2, blockX3(a), blockY3(a), a)
    if didUndo = 1 and (blockX(a) <> blockX2(a) or blockY(a) <> y2) then
        didUndo = 2 ''Keep the undo file we saved earlier, since something changed
    end if
next a

if didUndo = 1 and UndoNumber > -1 then ''if nothing actually resulted from the keypress, discard the last undo point
    undoNumber -= 1
    if undoNumber < 0 then undoNumber = maxUndoLevels - 1
    if undoStart <> -1 then
        undoStart -= 1
        if undoStart < 0 then undoStart = maxUndoLevels - 1
    end if
end if

''(Temporary Debug) check if any blocks had gravity applied more than twice (one for each axis) to them
for a = 0 to numBlocks - 1
    if blockDidGravity(a) > 2 then
        beep
        print "Block"; a; " had gravity applied"; blockDidGravity(a); " times."
        sleep
    end if
next a


''Move blocks with players
'for a = 0 to numBlocks - 1
'    if blockX(a) <> blockX2(a) then
'        b = playerBlock(a)
'        if b <> -1 then
'            if blockPath(b) = "" and blockXMoving(b) = 0 and blockYMoving(b) = 0 then
'                'if BoxClear(blockX(a), blockY(a) - blockH(b), blockW(b), blockH(b)) then
'                    'blockX(b) += (blockX(a) - blockX2(a))
'                    ''blockY(b) = blockY(a) - blockH(b)
'                'end if
'            end if
'        end if
'    end if
'next a
'

'for a = 0 to numBlocks - 1
'    Physics(blockX(a), blockY(a), blockW(a), blockH(a), 0, 0, 0, 0, "", 0, 0, blockX2(a), blockY2(a), blockX3(a), blockY3(a), blockType(a), a)
'next a


end sub







function BoxClear (byval x as integer, byval y as integer, byval w as integer, byval h as integer) as byte
dim as integer a
'line (x, y)-(x + (w-1), y +(h-1)), rgb(255,0,0), b
BoxClear = 1
for a = 0 to numBlocks - 1
    if BoxOverlap(x, y, w, h, blockX(a), blockY(a), blockW(a), blockH(a)) then
        BoxClear = 0
        exit function
    end if
next a
for a = 0 to numBricks - 1
    if BoxOverlap(x, y, w, h, brickX(a), brickY(a), brickW(a), brickH(a)) then
        'line(x, y)-step(w - 1, h - 1), rgb(255, 0, 0), b 
        'flip
        'sleep
        
        BoxClear = 0
        exit function
    end if
next a
end function








function BoxOverlap (byval x1 as integer, byval y1 as integer, byval w1 as integer, byval h1 as integer, byval x2 as integer, byval y2 as integer, byval w2 as integer, byval h2 as integer) as byte
BoxOverlap = 0
if x1 <= x2 + (w2 - 1) and x1 + (w1 - 1) >= x2 and y1 <= y2 + (h2 - 1) and y1 + (h1 - 1) >= y2 then
    BoxOverlap = 1
end if
end function












sub CheckForStep (byval a as integer) ''Checks if player should step over a block, and adds the path for it
static y as integer
static j as integer

if blockDir(a) = 1 then
    if BoxClear(blockX(a) + (blockDir(a) * blockW(a)), blockY(a), 1, blockH(a)) = 1 then exit sub
end if
if blockDir(a) = -1 then
    if BoxClear(blockX(a) - 1, blockY(a), 1, blockH(a)) = 1 then exit sub
end if

for y = blockY(a) to blockY(a) - tileH - (blockY(a) mod tileH) step -1
    if BoxClear(blockX(a) + (blockW(a) * blockDir(a)), y, blockW(a), blockH(a)) = 1 then
        j = y - blockY(a)
        'locate 19, 1: print tileH mod blockY(a); "  "
        'locate 20, 1
        'print j;"  ":sleep
        blockPath(a) = str(j) + "y" + str(blockW(a) * blockDir(a)) + "x"
        exit sub
    end if
next y
end sub







sub DeleteTempFolder
dim as string s

do
    s = Dir("temp\*")
    'print "del '"; s; "'"
    'sleep
    if s <> "" then
        kill "temp\" + s
    else
        exit do
    end if
loop

rmdir "temp"

end sub





sub DrawBlocks
static t as double, blink as byte
dim as integer a, x, y

for a = 0 to numBlocks - 1
    get (blockX(a), blockY(a))-step(tileW - 1, tileH - 1), blockErase(a, 0)
next a

if timer >= t + .1 or timer < t then blink = 1 else blink = 0

for a = 0 to numBlocks - 1
    x = blockX(a)
    y = blockY(a)
    if blockType(a) = 0 then
        if blink then
            if int(rnd * 700) = 0 then
                if blockMouth(a) = 0 then blockMouth(a) = 1 else blockMouth(a) = 0
            end if
        end if
        if blockMouth(a) = 0 then put (x, y), block0(0), trans
        if blockMouth(a) = 1 then put (x, y), block1(0), trans
    end if
    if blockType(a) = 1 then
        if blockFalling(a) = 1 then
            ''Falling
            select case blockDir(a)
                case -1
                    put(x, y), aelio0_4(0), trans
                case 0
                    put(x, y), aelio2_4(0), trans
                case 1
                    put(x, y), aelio1_4(0), trans
            end select
        else
            if blink = 1 and int(rnd * 30) = 0 then
                playerBlink(a) = timer
            end if
            select case blockDir(a)
                case -1
                    if timer < playerBlink(a) + .1 then put(x, y), aelio0_2(0), trans else put(x, y), aelio0_0(0), trans
                case 0
                    if timer < playerBlink(a) + .1 then put(x, y), aelio2_2(0), trans else put(x, y), aelio2_0(0), trans
                case 1
                    if timer < playerBlink(a) + .1 then put(x, y), aelio1_2(0), trans else put(x, y), aelio1_0(0), trans
            end select
            if timer < playerBlink(a) then playerBlink(a) = timer
        end if
    end if
next a

if blink = 1 then t = timer

end sub





'sub DrawString (x as integer, y as integer, w as integer, s as string, zoomX as integer, zoomY as integer)
'static as integer a, b, c, z, h
'
'
'h = len(s) / w
'
'z = 0
'for b = y to (y + (h - 1)) * zoomY step zoomY
'    for a = x to (x + (w - 1)) * zoomX step zoomX
'        z+=1
'        c = val(mid$(s, z, 1))
'        if mid$(s, z, 1) = " " then c = -1
'        'select case mid$(s, z, 1)
'        'case 0:
'        'end select
'        if c <> -1 then line (a, b)-(a + zoomX, b + zoomY), c, bf
'        if z = len(s) then exit for
'    next a
'    if z = len(s) then exit for
'next b
'
'end sub
'












sub DrawBricks
dim as integer a
for a = 0 to numBricks - 1
    put (brickX(a), brickY(a)), brick0(0), trans
next a
end sub






sub DrawTorches
dim as integer a
static as double t

if timer > t + .05 or timer < t then
    for a = 0 to numTorches - 1
        'torchFlame(a) = int(rnd * 6)
        if int(rnd * 2) = 0 then torchFlame(a) -= int(rnd * 2) + 1 else torchFlame(a) += int(rnd * 2) + 1
        if torchFlame(a) < 0 then torchFlame(a) += 2
        if torchFlame(a) > 7 then torchFlame(a) -= 2
    next a
    t = timer
end if

for a = 0 to numTorches - 1
    select case torchFlame(a)
    case 0
        put (torchX(a), torchY(a)), torch0_0(0), trans
    case 1
        put (torchX(a), torchY(a)), torch0_1(0), trans
    case 2
        put (torchX(a), torchY(a)), torch0_2(0), trans
    case 3
        put (torchX(a), torchY(a)), torch0_3(0), trans
    case 4
        put (torchX(a), torchY(a)), torch0_4(0), trans
    case 5
        put (torchX(a), torchY(a)), torch0_5(0), trans
    case 6
        put (torchX(a), torchY(a)), torch0_6(0), trans
    case 7
        put (torchX(a), torchY(a)), torch0_7(0), trans
    end select
next a

end sub




sub EraseBlocks
dim as integer a
for a = 0 to numBlocks - 1
    'line(blockX2(a), blockY2(a))-(blockX2(a) + (blockW(a) - 1), blockY2(a) + (blockH(a) - 1)), 0 ,bf
    put (blockX(a), blockY(a)), blockErase(a, 0), pset
next a

end sub


















'sub MoveBlocksOnTop (byval a as integer)
'dim as integer b
'
'b = BlockNumber(blockX(a), blockY(a) - 1, blockW(a), 1)
'if b <> -1 then
'    blockXMoving(b) = blockXMoving(a)
'    MoveBlocksOnTop(b)
'end if
'end sub







sub Physics (byval axis as byte, byref x as integer, byref y as integer, byref w as integer, byref h as integer, byref xSpeed as integer, byref YSpeed as integer, byref XGravity as integer, byref YGravity as integer, byref path as string, byref XMoving as byte, byref YMoving as byte, byref x2 as integer, byref y2 as integer, byref x3 as integer, byref y3 as integer, byref blockNum as integer)
''axis: 1 or -1 is X     2 or -2 is Y     3 or -3 is both X and Y      > 0 is gravity
dim as integer b, c, newX, x1, newY, y1, blockList(numBlocks - 1), listNum
dim as byte movedX, hitX, movedY, hitY, ok

'AdvancePaths(x, y, x3, y3, XMoving, YMoving, path)

x2 = x
y2 = y

newX = x
if axis > 0 then
    if XMoving = 0 and YMoving = 0 then
        newX = x + XGravity
        blockDidGravity(blockNum) += 1
    end if
end if
if abs(axis) = 1 or abs(axis) = 3 then
    if XMoving > 0 then
        newX = x + XSpeed
        if newX > x + XMoving then newX = x + XMoving
    end if
    if XMoving < 0 then
        newX = x - XSpeed
        if newX < x + XMoving then newX = x + XMoving
    end if
    if XMoving <> 0 then ''Decrease remaining pixels to move next time
        if XMoving < 0 then
            XMoving += Xspeed
            if XMoving > 0 then XMoving = 0
        else
            XMoving -= XSpeed
            if XMoving < 0 then XMoving = 0
        end if
        'if abs(newX - x3) > abs(XMoving) then
        '    newX = x3 + XMoving
        '    XMoving = 0
        'end if
    end if
end if
movedX = 0


if newX > x then
    hitX = 0
    for x1 = x to newX
        for b = 0 to numBricks - 1
            if BoxOverlap(x1, y, w, h, brickX(b), brickY(b), brickW(b), brickH(b)) <> 0 then
                x = brickX(b) - w
                hitX = 1
                exit for
            end if
        next b
        if hitX = 0 then
            listNum = -1
            for b = 0 to numBlocks - 1
                if b <> blockNum then
                    if BoxOverlap(x1, y, w, h, blockX(b), blockY(b), blockW(b), blockH(b)) <> 0 then
                        ok = 0
                        if blockStrong(blockNum) = 2 then
                            blockStrong(b) = 2
                            
                            ''See if there's room to move this block to the right
                            if RoomRightBlock(b, 1) = 1 then                            
                                ''Add block to list of blocks to move over to right
                                listNum += 1
                                blockList(listNum) = b
                                ok = 1
                                
                                'DrawBlocks
                                'line (blockX(b), blockY(b))-step(blockW(b) - 1, blockH(b) - 1), rgb(255, 0, 0), b
                                'flip
                                'EraseBlocks
                                'sleep
                            end if
                        end if

                        ''If this block didn't have room to move over
                        if ok = 0 then
                            listNum = -1 ''Don't move any of the previously-found blocks over
                            x = blockX(b) - w
                            hitX = 1
                            exit for
                        end if
                    end if
                end if
            next b

            ''Move previously-found blocks over to the right
            if listNum > -1 then
                for b = 0 to listNum
                    blockXMoving(blockList(b)) = 1
                    Physics(-1, blockX(blockList(b)), blockY(blockList(b)), blockW(blockList(b)), blockH(blockList(b)), blockXSpeed(blockList(b)), blockYSpeed(blockList(b)), blockXGravity, blockYGravity, blockPath(blockList(b)), blockXMoving(blockList(b)), blockYMoving(blockList(b)), blockX2(blockList(b)), blockY2(blockList(b)), blockX3(blockList(b)), blockY3(blockList(b)), blockList(b))
                next b
            end if            
        end if
        if hitX <> 0 then exit for
    next x1
    if hitX = 0 then x = newX
    movedX = 1
end if


if newX < x and movedX = 0 then
    hitX = 0
    for x1 = x to newX step - 1
        for b = 0 to numBricks - 1
            if BoxOverlap(x1, y, w, h, brickX(b), brickY(b), brickW(b), brickH(b)) <> 0 then
                x = brickX(b) + brickW(b)
                hitX = 1
                exit for
            end if
        next b
        if hitX = 0 then
            listNum = -1
            ''Check for other blocks that are blocking the current blockNum
            for b = 0 to numBlocks - 1
                if b <> blockNum then
                    if BoxOverlap(x1, y, w, h, blockX(b), blockY(b), blockW(b), blockH(b)) <> 0 then
                        ok = 0
                        if blockStrong(blockNum) = 2 then
                            blockStrong(b) = 2
                                                        
                            ''See if there's room to move this block to the left
                            if RoomLeftBlock(b, 1) = 1 then
                                ''Add block to list of blocks to move over to left
                                listNum += 1
                                blockList(listNum) = b
                                ok = 1
                            end if
                        end if
                            
                        ''If this block didn't have room to move over
                        if ok = 0 then
                            listNum = -1 ''Don't move any of the previously-found blocks over
                            x = blockX(b) + blockW(b)
                            hitX = 1
                            exit for
                        end if
                    end if
                end if
            next b
            
            ''Move previously-found blocks over to the left
            if listNum > -1 then
                for b = 0 to listNum
                    blockXMoving(blockList(b)) = -1
                    Physics(-1, blockX(blockList(b)), blockY(blockList(b)), blockW(blockList(b)), blockH(blockList(b)), blockXSpeed(blockList(b)), blockYSpeed(blockList(b)), blockXGravity, blockYGravity, blockPath(blockList(b)), blockXMoving(blockList(b)), blockYMoving(blockList(b)), blockX2(blockList(b)), blockY2(blockList(b)), blockX3(blockList(b)), blockY3(blockList(b)), blockList(b))
                next b
            end if
        end if
        if hitX <> 0 then exit for
    next x1
    if hitX = 0 then x = newX
end if

if hitX = 1 then
    XMoving = 0
    if abs(x3 - x) < abs(XMoving) then path = ""': print "X END"
end if

if x <> x2 then ''If this block moved, move the blocks on top of it
    b = BlockNumber(x, y - 1, w, 1)
    if b <> -1 then
        if blockX(b) = x2 then
            'locate 2, 1
            'print "Block "; str(blockNum); " moved "; str(x - x2); "px on x-axis to "; x; "    "
            'print "Block "; str(b); " is on top of it, and is being moved by the same amount  "
            'print timer; "     "
            'flip : sleep
            
            blockXMoving(b) = x - x2'blockXMoving(a)
            Physics(-1, blockX(b), blockY(b), blockW(b), blockH(b), blockXSpeed(b), blockYSpeed(b), blockXGravity, blockYGravity, blockPath(b), blockXMoving(b), blockYMoving(b), blockX2(b), blockY2(b), blockX3(b), blockY3(b), b)
            '' Jesus is Lord.  Thank you Jesus for sacrificing yourself for me!
        end if
    end if
end if

'if XMoving = 0 and blockStrong(blockNum) = 1 and blockType(blockNum) <> 1 then
'    blockStrong(blockNum) = 0
'end if


'AdvancePaths(x, y, x3, y3, XMoving, YMoving, path)

newY = y

if axis > 0 then
    if XMoving = 0 and YMoving = 0 then
        newY = y + YGravity
        blockDidGravity(blockNum) += 1
    end if
end if
if abs(axis) = 2 or abs(axis) = 3 then
    if YMoving > 0 then
        newY = y + YSpeed
        if newY > y + YMoving then newY = y + YMoving
    end if
    if YMoving < 0 then
        newY = y - YSpeed
        if newY < y + YMoving then newY = y + YMoving
    end if
    if YMoving <> 0 then
        if YMoving < 0 then
            YMoving += Yspeed
            if YMoving > 0 then YMoving = 0
        else
            YMoving -= YSpeed
            if YMoving < 0 then YMoving = 0
        end if
        'if abs(newY - y3) > abs(YMoving) then
        '    newY = y3 + YMoving
        '    YMoving = 0
        'end if
    end if
end if

movedY = 0

if newY > y then
    hitY = 0
    for y1 = y to newY
        for b = 0 to numBricks - 1
            if BoxOverlap(x, y1, w, h, brickX(b), brickY(b), brickW(b), brickH(b)) <> 0 then
                y = brickY(b) - h
                hitY = 1
                exit for
            end if
        next b
        if hitY = 0 then
            for b = 0 to numBlocks - 1
                if b <> blockNum then
                    if BoxOverlap(x, y1, w, h, blockX(b), blockY(b), blockW(b), blockH(b)) <> 0 then
                        ok = 0
                            
                        ''If the colliding block hasn't had its gravity applied to it yet, do that
                        if blockDidGravity(b) = 0 then
                            Physics(4, blockX(b), blockY(b), blockW(b), blockH(b), blockXSpeed(b), blockYSpeed(b), blockXGravity, blockYGravity, blockPath(b), blockXMoving(b), blockYMoving(b), blockX2(b), blockY2(b), blockX3(b), blockY3(b), b)
                            ''If it moved, then there was no collision
                            if blockY(b) > blockY2(b) then ok = 1
                        end if
                                                                        
                        if ok = 0 then
                            y = blockY(b) - h
                            hitY = 1
                            exit for
                        end if
                        'exit for
                    end if
                end if
            next b
        end if
        if hitY <> 0 then exit for
    next y1
    if hitY = 0 then y = newY
    movedY = 1
end if


if newY < y and movedY = 0 then
    hitY = 0
    for y1 = y to newY step - 1
        for b = 0 to numBricks - 1
            if BoxOverlap(x, y1, w, h, brickX(b), brickY(b), brickW(b), brickH(b)) <> 0 then
                y = brickY(b) + brickH(b)
                hitY = 1
                exit for
            end if
        next b
        if hitY = 0 then
            listNum = -1
            for b = 0 to numBlocks - 1
                if b <> blockNum then
                    if BoxOverlap(x, y1, w, h, blockX(b), blockY(b), blockW(b), blockH(b)) <> 0 then
                        ok = 0
                        if blockStrong(blockNum) > 0 or blockType(blockNum) = 1 then
                            if blockStrong(b) = 0 then blockStrong(b) = 2
                            
                            ''If it's a normal-strength player pushing up on a block, let it try moving only one block up
                            if blockStrong(blockNum) <= 0 and blockType(blockNum) = 1 then c = 0 else c = 1

                            ''See if there's room to move this block up
                            if RoomAboveBlock(b, 1, c) = 1 then                            
                                ''Add block to list of blocks to move up
                                listNum += 1
                                blockList(listNum) = b
                                ok = 1
                            end if
                        end if
                                                   
                        ''If this block didn't have room to move up
                        if ok = 0 then
                            listNum = -1 ''Don't move any of the previously-found blocks over
                            y = blockY(b) + blockH(b)
                            hitY = 1
                            exit for
                        end if
                    end if
                end if
            next b
            
            ''Move previously-found blocks up
            if listNum > -1 then
                for b = 0 to listNum
                    blockYMoving(blockList(b)) = -1
                    Physics(-2, blockX(blockList(b)), blockY(blockList(b)), blockW(blockList(b)), blockH(blockList(b)), blockXSpeed(blockList(b)), blockYSpeed(blockList(b)), blockXGravity, blockYGravity, blockPath(blockList(b)), blockXMoving(blockList(b)), blockYMoving(blockList(b)), blockX2(blockList(b)), blockY2(blockList(b)), blockX3(blockList(b)), blockY3(blockList(b)), blockList(b))
                next b
            end if            
        end if
        if hitY <> 0 then exit for
    next y1
    if hitY = 0 then y = newY
end if

if hitY = 1 then
    YMoving = 0
    if path <> "" then
        if abs(y3 - y) < abs(YMoving) then path = ""': print "Y END"
    end if
end if

if XMoving = 0 and YMoving = 0 and blockStrong(blockNum) = 2 then
    blockStrong(blockNum) = 0
end if

if multiKey(sc_1) then
    DrawBlocks
    flip
    for c = 0 to 120
        for b = 0 to 30000
        next b
    next c
    EraseBlocks
end if

end sub






































function BlockNumber (x as integer, y as integer, w as integer, h as integer) as integer
dim as integer a
BlockNumber = -1
for a = 0 to numBlocks - 1
    if BoxOverlap(x, y, w, h, blockX(a), blockY(a), blockW(a), blockH(a)) then
        BlockNumber = a
        exit function
    end if
next a
end function






function BrickNumber (x as integer, y as integer, w as integer, h as integer) as integer
dim as integer a
BrickNumber = -1
for a = 0 to numBricks - 1
    if BoxOverlap(x, y, w, h, brickX(a), brickY(a), brickW(a), brickH(a)) then
        BrickNumber = a
        exit function
    end if
next a
end function








function PlayerNumber (x as integer, y as integer) as byte
dim as integer a

PlayerNumber = -1
for a = 0 to numBlocks - 1
    if blockType(a) = 1 then
        if BoxOverlap(x, y, 1, 1, blockX(a), blockY(a), blockW(a), blockH(a)) then
            PlayerNumber = a
            exit function
        end if
    end if
next a
end function







sub SaveUndo
dim as integer a
dim as string s

undoNumber += 1
if undoStart <> -1 then
    if undoNumber = undoStart then undoStart += 1
    if undoStart > maxUndoLevels - 1 then undoStart = 0
end if

if undoNumber > maxUndoLevels - 1 then
    undoNumber = 0
    undoStart = 1
end if

open undoFile + "." + trim(str(undoNumber)) for output as #1
    'print #1, ":"; ltrim(str(undoNumber))
    print #1, numPlayers
    print #1, numBlocks
    for a = 0 to numBlocks - 1
        print #1, str(blockX(a))
        print #1, str(blockY(a))
        print #1, str(blockX2(a))
        print #1, str(blockY2(a))
        print #1, str(blockX3(a))
        print #1, str(blockY3(a))
        print #1, str(blockW(a))
        print #1, str(blockH(a))
        print #1, str(blockXMoving(a))
        print #1, str(blockYMoving(a))
        print #1, str(blockDir(a))
        print #1, blockPath(a)
        print #1, str(blockStrong(a))
    next a
    
    'open tempFile for input as #2
    '    do while not eof(2)
    '        line input #2, s
    '        print #1, s
    '    loop
    'close #2
close #1
end sub






sub LoadUndo
dim as integer a, l
dim as string s

if undoStart <> -1 then
    'if undoNumber = undoStart then exit sub
else
    if undoNumber < 0 then exit sub
end if

open undoFile + "." + trim(str(undoNumber)) for input as #1
    input #1, numPlayers
    input #1, numBlocks
    for a = 0 to numBlocks - 1
        input #1, blockX(a)
        input #1, blockY(a)
        input #1, blockX2(a)
        input #1, blockY2(a)
        input #1, blockX3(a)
        input #1, blockY3(a)
        input #1, blockW(a)
        input #1, blockH(a)
        input #1, blockXMoving(a)
        input #1, blockYMoving(a)
        input #1, blockDir(a)
        line input #1, blockPath(a)
        input #1, blockStrong(a)
    next a    
close #1

'open tempFile for input as #1
'    open undoFile for output as #2
'        do while not eof(1)  
'            line input #1, s
'            print #2, s
'        loop
'    close #2
'close #1
'kill tempFile
'
'cls
'screenLock
'DrawBricks

undoNumber -= 1
if undoStart <> -1 then
    if undoNumber = undoStart - 1 then undoNumber = undoStart
    if undoNumber < 0 then undoNumber = maxUndoLevels - 1
end if

end sub









sub QuitGame
DeleteTempFolder
end
end sub







function RoomAboveBlock(byval a as integer, byval p as integer, byval recursive as byte) as byte
dim as integer b

RoomAboveBlock = 0
if BoxClear(blockX(a), blockY(a) - p, blockW(a), p) then
    'blockY(a) -= p
    RoomAboveBlock = 1
    exit function
end if

if recursive = 0 then exit function

b = BrickNumber(blockX(a), blockY(a) - p, blockW(a), p)
if b = -1 then
    RoomAboveBlock = 1
    for b = 0 to numBlocks - 1
        if BoxOverlap(blockX(a), blockY(a) - p, blockW(a), p, blockX(b), blockY(b), blockW(b), blockH(b)) then
            if RoomAboveBlock(b, p, 1) = 0 then
                RoomAboveBlock = 0
                exit function
            end if
        end if
    next b
end if

'b = BrickNumber(blockX(a), blockY(a) - p, blockW(a), p)
'if b = -1 then
'    b = BlockNumber(blockX(a), blockY(a) - p, blockW(a), p)
'    if b <> -1 then
'        if RoomAboveBlock(b, p, 1) then
'            'blockY(a) -= p
'            RoomAboveBlock = 1
'        end if
'    end if
'end if
end function






function RoomRightBlock(byval a as integer, byval p as integer) as byte
dim as integer b

RoomRightBlock = 0
if BoxClear(blockX(a) + (blockW(a) - 1) + p, blockY(a), p, blockH(a)) then
    'blockX(a) += p
    RoomRightBlock = 1
    exit function
end if


b = BrickNumber(blockX(a) + (blockW(a) - 1) + p, blockY(a), p, blockH(a))
if b = -1 then
    RoomRightBlock = 1
    for b = 0 to numBlocks - 1
        if BoxOverlap(blockX(a) + (blockW(a) - 1) + p, blockY(a), p, blockH(a), blockX(b), blockY(b), blockW(b), blockH(b)) then
            if RoomRightBlock(b, p) = 0 then
                RoomRightBlock = 0
                exit function
            end if
        end if
    next b
end if

'b = BrickNumber(blockX(a) + (blockW(a) - 1) + p, blockY(a), p, blockH(a))
'if b = -1 then
'    b = BlockNumber(blockX(a) + (blockW(a) - 1) + p, blockY(a), p, blockH(a))
'    if b <> - 1 then
'        if RoomRightBlock(b, p) then
'            'blockX(a) += p
'            RoomRightBlock = 1
'        end if
'    end if
'end if
end function







function RoomLeftBlock(byval a as integer, byval p as integer) as byte
dim as integer b

'    DrawBlocks
'    line(blockX(a) - p, blockY(a))-step(p - 1, blockH(a) - 1), rgb(255, 0, 0), b
'    flip
'    EraseBlocks
'    sleep

RoomLeftBlock = 0
if BoxClear(blockX(a) - p, blockY(a), p, blockH(a)) = 1 then
    'blockX(a) -= p
    RoomLeftBlock = 1
    exit function
end if


b = BrickNumber(blockX(a) - p, blockY(a), p, blockH(a))
if b = -1 then
    RoomLeftBlock = 1
    for b = 0 to numBlocks - 1
        if BoxOverlap(blockX(a) - p, blockY(a), p, blockH(a), blockX(b), blockY(b), blockW(b), blockH(b)) then
            if RoomLeftBlock(b, p) = 0 then
                RoomLeftBlock = 0
                exit function
            end if
        end if
    next b
end if

'b = BrickNumber(blockX(a) - p, blockY(a), p, blockH(a))
'if b = -1 then
'    b = BlockNumber(blockX(a) - p, blockY(a), p, blockH(a))
'    if b <> -1 then
'        if RoomLeftBlock(b, p) = 1 then
'            'blockX(a) -= p
'            RoomLeftBlock = 1
'        end if
'    end if
'end if

end function










