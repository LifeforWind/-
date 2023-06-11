#include "Tetris.h"
//const int SPEED_NORMAL = 500;//ms
const int SPEED_QUICK = 50;
const int SPEED_N[5] = { 500,300,150,100,80 };
#define RECORDER_FILE "recorder.txt"

Tetris::Tetris(int _rows, int _cols, int left, int top, int _blockSize)//����ĳ�ʼ����Ҫ�ٿ��ǣ���Ϊ�п��ܻ�����޷���ڶ��ε����
{
    rows = _rows;
    cols = _cols;
    leftMargin = left;
    topMargin = top;
    blockSize = _blockSize;
    //��ʼ����ͼ����,��ͼ�е�Ԫ�����Ϊ0�����޷��飬�����5�����5�ֶ���˹�����е�С����
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
    mciSendString("play res/bg.mp3 repeat", 0, 0, 0);//���ű�������

    delay = SPEED_N[0];//������Կ��Ǹ���,�����⣬���delay�����Ǳ�ʾʲô

    //�����������,����Ҳ�������㻨��
    srand(time(NULL));

    //������Ϸ����,�������ͨ���ı�initgraph�Ĳ�����ʵ�ֱ��������Ĺ���
    initgraph(938,896);

    //���ر���ͼƬ
    loadimage(&imgBk, "res/bg2.png");

    loadimage(&imgWin, "res/win.png");
    loadimage(&imgOver, "res/over.png");

    //���³�ʼ������
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

    //��ʼ����߷�
    ifstream file(RECORDER_FILE);
    if (!file.is_open()) {
        cout << RECORDER_FILE << "��ʧ��" << endl;
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

        //�ж��Ƿ���ͣ
        if (pause) {
            keyEvent();//���������������Ĳ���
            
            if (saveKey) saveProgress(practice);
            if (loadKey)loadProgress(practice);
            continue;
        }
        //��Ȼ��Ͳ�����ͣ״̬��

        keyEvent(); //�����û�������

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
            //�������
            saveScore();

            //���½�������
            displayOver();

            system("pause");
            init();//���¿�ʼ
        }
    }
}

void Tetris::keyEvent()
{
    int ch1=0,ch2=0;
    bool rotateFlag=false;
    int dx = 0;
    //�ж��Ƿ���������ͣ
    //��һ�������û����ͣ��״̬��������ʱ���⵽Pֱ����ͣ
    //�ڶ���������Ѿ���ͣ�ˣ�������Ҫ����P�������ͣ״̬�������������i��浵������o�����
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
            //���������ӹ��ܣ�����Ϸ�����������������Ը����淨
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

    //�½������У����в��Ϸ����ж�
    if (curBlock->blockInMap(map) == false) {
        //����̻�
        bakBlock.solidify(map);
        delete curBlock;//��ֹ�ڴ�й©
        curBlock = nextBlock;
        nextBlock = new Block;

        //�����Ϸ�Ƿ��Ѿ�����
        checkOver();
        
    }

    delay = SPEED_N[level-1];
}

void Tetris::updateWindow()
{
    putimage(0, 0, &imgBk);
    
    IMAGE** imgs = Block::getImages();
    BeginBatchDraw();

    //�����ͼ���������Ԫ��Ϊĳһ����Ϊ0��������ʾ�����Ѿ��̻���
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (map[i][j] == 0)continue;

            int x = j * blockSize + leftMargin;
            int y = i * blockSize + topMargin;
            putimage(x, y, imgs[0]);//����Ҳ��Ҫ���ǣ������ں���ת��������ɫ���͵�ʱ����в���
        }
    }

    curBlock->draw(leftMargin, topMargin);
    nextBlock->draw(689, 150);//������Ը���Ϊ�����������ʹ�ں��汳���������ܷ����Ż�

    drawScore();//���Ʒ���

    EndBatchDraw();
}

void Tetris::clearLine()
{
    int lines = 0;
    int k = rows - 1;//�洢��������
    for (int i = rows - 1; i >= 0; i--) {
        //����i���ǲ�������
        int count = 0;
        for (int j = 0; j < cols; j++) {
            if (map[i][j])count++;
            map[k][j] = map[i][j];//һ��ɨ��һ�ߴ洢
        }

        if (count < cols) {
            //�����У������д洢����
            k--;
        }
        else {
            //����
            lines++;
            
        }
    }
    if (lines > 0) {
        //����÷�,����о��������������ǼƷֹ���
        int addScore[4] = { 10,30,60,80 };
        score += addScore[lines - 1];

        mciSendString("play res/xiaochu1.mp3", 0, 0, 0);
        update = true;

        //ÿ100��һ������0-100��һ�أ�101-200��2��
        level = (score + 99) / 100;
        if (level > 5) {
            gameOver = true;//ע�����ﲢ�������ǵ�Ҫ��ֻ���Ȳο�
        }

        lineCount += lines;
    }
}

void Tetris::moveLeftRight(int offset)
{
    bakBlock = *curBlock;
    //������������ƶ�������ͬʱҲҪ�ж���ײ,�������ﲢ���ǹ̻�����˲��ÿ����ڴ�й©
    curBlock->moveLeftRight(offset);

    if (!curBlock->blockInMap(map)) {
        *curBlock = bakBlock;
    }
}

void Tetris::rotate()
{
    if (curBlock->getBlocktype() == 7)return;//������ֱ�ӷ���

    bakBlock = *curBlock;
    curBlock->rotate();

    //��תҲҪ�ж��ܲ���ת
    if (!curBlock->blockInMap(map)) {
        *curBlock = bakBlock;
    }
}

void Tetris::drawScore()
{
    char scoreText[32];
    sprintf_s(scoreText, sizeof(scoreText), "%d", score);
    //������ʵ��һ�Ѵ��룬���Կ��ǰ���Щ�Ž����������Ӹ����ԣ�Ҳ���ܻ�ʵ�ָ�����ʽ����
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


    //��ӡĿǰ��������
    sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
    gettextstyle(&f);
    int xPos = 224 - f.lfWidth * strlen(scoreText);
    outtextxy(xPos, 817, scoreText);

    //���Ƶ�ǰ�ؿ���
    sprintf_s(scoreText, sizeof(scoreText), "%d", level);
    outtextxy(224 - 30, 727, scoreText);

    //������߷�
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

    //��������������ж�, win or lose
    if (level <= 5) {
        putimage(262, 361, &imgOver);
        mciSendString("play res/over.mp3", 0, 0, 0);
    }

    else {
        putimage(262, 361, &imgOver);
        mciSendString("play res/win.mp3", 0, 0, 0);
    }
}

void Tetris::beingPaused()//���������Ϊ����ͣ��ʱ��չʾ�Ķ���
{
    while (pause) {

    }
}

void Tetris::saveProgress(const string& filename)
{
    ofstream file(filename,ios::app);

    if (!file.is_open()) {
        cout << "�޷������浵�ļ� " << filename << endl;
        return;
    }

    // ������Ϸ״̬
    file << score << endl;
    file << lineCount << endl;
    file << level << endl;
    file << highestScore << endl;
    file << gameOver << endl;
    file << pause << endl;

    // �����ͼ����
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file << map[i][j] << " ";
        }
        file << endl;
    }

    // ���浱ǰ�������һ�������״̬
    file << curBlock->getBlocktype() << " " << curBlock->getBlockPos() << " "  << endl;
    file << nextBlock->getBlocktype() << endl;

    file.close();
}

void Tetris::loadProgress(const string& filename)
{
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "�޷��򿪴浵�ļ� " << filename << endl;
        return;
    }

    // ��ȡ��Ϸ״̬
    file >> score;
    file >> lineCount;
    file >> level;
    file >> highestScore;
    file >> gameOver;
    file >> pause;

    // ��ȡ��ͼ����
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file >> map[i][j];
        }
    }

    // ��ȡ��ǰ�������һ�������״̬
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