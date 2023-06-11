#include "Tetris.h"
//const int SPEED_NORMAL = 500;//ms
const int SPEED_QUICK = 50;
const int SPEED_N[5] = { 500,300,150,100,80 };
#define RECORDER_FILE "recorder.txt"

Tetris::Tetris(int _rows, int _cols, int left, int top, int _blockSize)//这里的初始化需要再考虑，因为有可能会出现无法玩第二次的情况
{
    rows = _rows;
    cols = _cols;
    leftMargin = left;
    topMargin = top;
    blockSize = _blockSize;
    //初始化地图数组,地图中的元素如果为0代表无方块，如果是5代表第5种俄罗斯方块中的小方块
    for (int i = 0; i < rows; i++) {
        vector<int>mapRow;
        for (int j = 0; j < cols; j++) {
            mapRow.push_back(0);
        }
        map.push_back(mapRow);
    }
}

void Tetris::init()
{
    mciSendString("play res/bg.mp3 repeat", 0, 0, 0);//播放背景音乐

    delay = SPEED_N[0];//这里可以考虑更改,有问题，这个delay到底是表示什么

    //配置随机种子,这里也可以整点花活
    srand(time(NULL));

    //创建游戏窗口,这里可以通过改变initgraph的参数来实现背景更换的功能
    initgraph(938,896);

    //加载背景图片
    loadimage(&imgBk, "res/bg2.png");

    loadimage(&imgWin, "res/win.png");
    loadimage(&imgOver, "res/over.png");

    //重新初始化数据
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            map[i][j] = 0;
        }
    }
    score = 0;
    lineCount = 0;
    level = 1;
    gameOver = false;
    pause = false;
    loadKey = false;
    saveKey = false;

    //初始化最高分
    ifstream file(RECORDER_FILE);
    if (!file.is_open()) {
        cout << RECORDER_FILE << "打开失败" << endl;
        highestScore = 0;
    }
    else {
        file >> highestScore;
    }
    file.close();
}

void Tetris::game()
{
    init();

    nextBlock = new Block;
    curBlock = nextBlock;
    nextBlock = new Block;
    string practice="abc";
    int timer = 0;
    while (1) {

        //判断是否暂停
        if (pause) {
            keyEvent();//这里可以添加其他的操作
            
            if (saveKey) saveProgress(practice);
            if (loadKey)loadProgress(practice);
            continue;
        }
        //显然这就不是暂停状态下

        keyEvent(); //接受用户的输入

        timer += getDelay();
        if (timer > delay) { 
            timer = 0;
            drop();            
            update = true;
        }

        if (update) {
            update = false;
            updateWindow();  
            clearLine();     
        }

        if (gameOver) {
            //保存分数
            saveScore();

            //更新结束界面
            displayOver();

            system("pause");
            init();//重新开始
        }
    }
}

void Tetris::keyEvent()
{
    int ch1=0,ch2=0;
    bool rotateFlag=false;
    int dx = 0;
    //判断是否输入了暂停
    //第一种情况是没有暂停的状态，因此这个时候检测到P直接暂停
    //第二种情况是已经暂停了，我们需要按下P来解除暂停状态，这里如果按下i会存档，按下o会读档
    if (pause) {
        if (_kbhit()) {
            ch1 = _getch();
            if (ch1 == 105) {
                saveKey = true;
            }
            else if (ch1 == 111) {
                loadKey = true;
                pause = false;
            }
            else if (ch1 == 112) {
                pause = false;
            }
        }

    }

    else {
        if (_kbhit()) {
            ch1 = _getch();

            /*switch (ch) {
            case 97:
                dx = -1;
                break;
            case 100:
                dx = 1;
                break;
            default:
                break;
            }*/

            if (ch1 == 224) {
                ch2 = _getch();
                switch (ch2) {
                case 72:
                    rotateFlag = true;
                    break;
                case 80:
                    delay = SPEED_QUICK;
                    break;
                case 75:
                    dx = -1;
                    break;
                case 77:
                    dx = 1;
                    break;
                default:
                    break;
                }
            }
            //这里可以添加功能，与游戏设置链接起来，可以更改玩法
            else if (ch1 == 112) {
                pause = true;
            }
        }
    }

    if (rotateFlag) {
        rotate();
        update = true;
    }

    if (dx != 0) {
        moveLeftRight(dx);
        update = true;
    }
}

int Tetris::getDelay()
{
    static unsigned long long lastTime = 0;
    unsigned long long curTime = GetTickCount();
    if (lastTime == 0) { 
        lastTime = curTime;
        return 0;
    }
    else {
        int r = curTime - lastTime;
        lastTime = curTime;
        return r;
    }

}

void Tetris::drop()
{
    bakBlock = *curBlock;
    curBlock->drop();

    //下降过程中，会有不合法的判断
    if (curBlock->blockInMap(map) == false) {
        //方块固化
        bakBlock.solidify(map);
        delete curBlock;//防止内存泄漏
        curBlock = nextBlock;
        nextBlock = new Block;

        //检查游戏是否已经结束
        checkOver();
        
    }

    delay = SPEED_N[level-1];
}

void Tetris::updateWindow()
{
    putimage(0, 0, &imgBk);
    
    IMAGE** imgs = Block::getImages();
    BeginBatchDraw();

    //如果地图数组里面的元素为某一个不为0的数，表示这里已经固化了
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (map[i][j] == 0)continue;

            int x = j * blockSize + leftMargin;
            int y = i * blockSize + topMargin;
            putimage(x, y, imgs[0]);//这里也需要考虑，方便在后期转换方块颜色类型的时候进行操作
        }
    }

    curBlock->draw(leftMargin, topMargin);
    nextBlock->draw(689, 150);//这里可以更改为传入参数，以使在后面背景更换功能方便优化

    drawScore();//绘制分数

    EndBatchDraw();
}

void Tetris::clearLine()
{
    int lines = 0;
    int k = rows - 1;//存储数据行数
    for (int i = rows - 1; i >= 0; i--) {
        //检查第i行是不是满的
        int count = 0;
        for (int j = 0; j < cols; j++) {
            if (map[i][j])count++;
            map[k][j] = map[i][j];//一边扫描一边存储
        }

        if (count < cols) {
            //不满行，把这行存储下来
            k--;
        }
        else {
            //满行
            lines++;
            
        }
    }
    if (lines > 0) {
        //计算得分,这里感觉可以整大活，尤其是计分规则
        int addScore[4] = { 10,30,60,80 };
        score += addScore[lines - 1];

        mciSendString("play res/xiaochu1.mp3", 0, 0, 0);
        update = true;

        //每100分一个级别，0-100第一关，101-200第2关
        level = (score + 99) / 100;
        if (level > 5) {
            gameOver = true;//注意这里并不是我们的要求，只是先参考
        }

        lineCount += lines;
    }
}

void Tetris::moveLeftRight(int offset)
{
    bakBlock = *curBlock;
    //这里调用左右移动函数，同时也要判断碰撞,但是这里并不是固化，因此不用考虑内存泄漏
    curBlock->moveLeftRight(offset);

    if (!curBlock->blockInMap(map)) {
        *curBlock = bakBlock;
    }
}

void Tetris::rotate()
{
    if (curBlock->getBlocktype() == 7)return;//田字形直接返回

    bakBlock = *curBlock;
    curBlock->rotate();

    //旋转也要判断能不能转
    if (!curBlock->blockInMap(map)) {
        *curBlock = bakBlock;
    }
}

void Tetris::drawScore()
{
    char scoreText[32];
    sprintf_s(scoreText, sizeof(scoreText), "%d", score);
    //以下其实有一堆代码，可以考虑把这些放进函数里增加复用性，也可能会实现更换样式功能
    setcolor(RGB(180, 180, 180));

    LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 60;
    f.lfWidth = 30;
    f.lfQuality = ANTIALIASED_QUALITY;
    strcpy_s(f.lfFaceName,sizeof(f.lfFaceName),_T("Segoe UI Black"));
    settextstyle(&f);

    setbkmode(TRANSPARENT);

    outtextxy(670, 727, scoreText);


    //打印目前消除行数
    sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
    gettextstyle(&f);
    int xPos = 224 - f.lfWidth * strlen(scoreText);
    outtextxy(xPos, 817, scoreText);

    //绘制当前关卡数
    sprintf_s(scoreText, sizeof(scoreText), "%d", level);
    outtextxy(224 - 30, 727, scoreText);

    //绘制最高分
    sprintf_s(scoreText, sizeof(scoreText), "%d", highestScore);
    outtextxy(670, 817, scoreText);
}

void Tetris::checkOver()
{
    gameOver = curBlock->blockInMap(map) == false;
}

void Tetris::saveScore()
{
    if (score > highestScore) {
        highestScore = score;

        ofstream file(RECORDER_FILE);
        file << highestScore;
        file.close();
    }
}

void Tetris::displayOver()
{
    mciSendString("stop res/bg.mp3", 0, 0, 0);

    //结束两种情况的判定, win or lose
    if (level <= 5) {
        putimage(262, 361, &imgOver);
        mciSendString("play res/over.mp3", 0, 0, 0);
    }

    else {
        putimage(262, 361, &imgOver);
        mciSendString("play res/win.mp3", 0, 0, 0);
    }
}

void Tetris::beingPaused()//这个可以作为在暂停的时候展示的东西
{
    while (pause) {

    }
}

void Tetris::saveProgress(const string& filename)
{
    ofstream file(filename,ios::app);

    if (!file.is_open()) {
        cout << "无法创建存档文件 " << filename << endl;
        return;
    }

    // 保存游戏状态
    file << score << endl;
    file << lineCount << endl;
    file << level << endl;
    file << highestScore << endl;
    file << gameOver << endl;
    file << pause << endl;

    // 保存地图数组
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file << map[i][j] << " ";
        }
        file << endl;
    }

    // 保存当前方块和下一个方块的状态
    file << curBlock->getBlocktype() << " " << curBlock->getBlockPos() << " "  << endl;
    file << nextBlock->getBlocktype() << endl;

    file.close();
}

void Tetris::loadProgress(const string& filename)
{
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "无法打开存档文件 " << filename << endl;
        return;
    }

    // 读取游戏状态
    file >> score;
    file >> lineCount;
    file >> level;
    file >> highestScore;
    file >> gameOver;
    file >> pause;

    // 读取地图数组
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file >> map[i][j];
        }
    }

    // 读取当前方块和下一个方块的状态
    int curType,a[8];
    int* cur=a;
    
    file >> curType >>*cur;
    curBlock = new Block();
    curBlock->setBlocktype(curType);
    curBlock->setBlockPos(cur);


    int nextType;
    file >> nextType;
    nextBlock = new Block();
    nextBlock->setBlocktype(nextType);

    file.close();
}