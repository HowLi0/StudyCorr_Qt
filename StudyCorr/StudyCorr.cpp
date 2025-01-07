#include "StudyCorr.h"

int StudyCorr::CalibrationIndex = 1;
int StudyCorr::ComputeIndex = 1;

StudyCorr::StudyCorr(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	resize(1200, 800);
	this->CalibrationIndex = 1;
	SetupUi(this->CalibrationIndex,this->ComputeIndex);
}

StudyCorr::~StudyCorr()
{}

void StudyCorr::SetupUi(int CalibrationIndex, int ComputeIndex)
{
	//****************************************************菜单栏****************************************************//
	//菜单栏创建,菜单栏最多只能有一个
	QMenuBar* Bar = menuBar();//菜单栏默认放在树中
	setMenuBar(Bar);//将菜单栏放在窗口中
	//创建菜单
	QMenu* FileMenu = Bar->addMenu("文件");
	QMenu* CalibrationMenu = Bar->addMenu("标定");
	QMenu* ComputeMenu = Bar->addMenu("计算");
	QMenu* DataMenu = Bar->addMenu("数据");
	QMenu* HelpMenu = Bar->addMenu("帮助");
	//创建菜单项
	QAction* NewAction = FileMenu->addAction("新建");
	NewAction->setIcon(QIcon("icon/1.png"));
	FileMenu->addSeparator();//添加分割线
	QAction* OpenAction = FileMenu->addAction("打开");
	OpenAction->setIcon(QIcon("icon/6.png"));

	// 连接信号和槽
	connect(NewAction, &QAction::triggered, this, &StudyCorr::CreateNewProject);
	connect(OpenAction, &QAction::triggered, this, &StudyCorr::OpenExistingProject);

	//****************************************************工具栏****************************************************//
	//添加工具栏
	QToolBar* ToolBar = new QToolBar(this);//工具栏默认不放在树中
	//addToolBar(ToolBar)
	addToolBar(Qt::TopToolBarArea, ToolBar);//更改停靠区域
	//设置允许停靠区域
	ToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::LeftToolBarArea | Qt::RightToolBarArea);
	//ToolBar->setMovable(false);
	//ToolBar->setFixedHeight(50);

	//工具栏加工具项
	ToolBar->addAction(NewAction);//接受菜单项，新建文件夹
	ToolBar->addSeparator();
	ToolBar->addAction(OpenAction);//接受菜单项，打开文件夹
	ToolBar->addSeparator();
	CalibrationButton = ToolBar->addAction("标定");//标定
	CalibrationButton->setIcon(QIcon("icon/7.png"));
	ToolBar->addSeparator();
	ComputeButton = ToolBar->addAction("计算");//计算
	ComputeButton->setIcon(QIcon("icon/8.png"));

	//连接信号和槽
	connect(CalibrationButton, &QAction::triggered, [=]() {
		this->CalibrationButtonClicked(this->CalibrationIndex);
		// 只有在第一次点击时才运行
		if (!hasRunCalibrationToolbars) {
			CalibrationToolBar();
			ChessToolBar();
			hasRunCalibrationToolbars = true;  // 设置标志为true，防止再次调用
		}
		this->CalibrationIndex ++;
		});

	connect(ComputeButton, &QAction::triggered, [=]() {
		this->ComputeButtonClicked(this->ComputeIndex);
		if (!hasRunComputeToolbars)
		{
			ComputeToolBar();
			hasRunComputeToolbars = true;
		}
		this->ComputeIndex++;
		});

//	//****************************************************状态栏****************************************************//
	//状态栏,最多有一个
	QStatusBar* stbar = new QStatusBar(this);
	//设置到窗口中
	setStatusBar(stbar);
	//放标签
	QLabel* label = new QLabel("NUAA", this);
	stbar->addWidget(label);
	QLabel* label2 = new QLabel("有问题请联系：476459099@qq.com", this);
	stbar->addPermanentWidget(label2);

	//****************************************************浮动窗口****************************************************//
	//铆接部件（浮动窗口），可以有多个
	QDockWidget* dock = new QDockWidget("工作区", this);
	dock->setFixedWidth(100);
	TabWidget = new QTabWidget(dock);
	// Create the first tree widget and add it to the first tab
	TreeWidget1 = new QTreeWidget(TabWidget);
	TreeWidget1->setHeaderHidden(true); // 隐藏顶部的header
	TabWidget->addTab(TreeWidget1, tr("标定"));

	// Create the second tree widget and add it to the second tab
	TreeWidget2 = new QTreeWidget(TabWidget);
	TreeWidget2->setHeaderHidden(true); // 隐藏顶部的header
	TabWidget->addTab(TreeWidget2, tr("计算"));

	dock->setWidget(TabWidget);
	addDockWidget(Qt::LeftDockWidgetArea, dock);

	//****************************************************图像显示/中心控件****************************************************//

// 创建一个 QGraphicsScene 以包含图像
	QGraphicsScene* scene1 = new QGraphicsScene(this);
	QGraphicsScene* scene2 = new QGraphicsScene(this);

	// 加载图像并添加到场景中
	QPixmap pixmap("C:/Users/admin/Pictures/Camera Roll/1.jpg");
	QGraphicsPixmapItem* item1 = new QGraphicsPixmapItem(pixmap);
	QGraphicsPixmapItem* item2 = new QGraphicsPixmapItem(pixmap);
	scene1->addItem(item1);
	scene2->addItem(item2);

	// 创建一个 QGraphicsView 以显示场景
	QGraphicsView* view1 = new QGraphicsView(scene1, this);
	QGraphicsView* view2 = new QGraphicsView(scene2, this);

	// 创建一个水平布局并添加两个 QGraphicsView 和分割线
	// 添加分割线，这里选择一个竖直方向的分割线
	QFrame* line = new QFrame(this);
	line->setFrameShape(QFrame::VLine); // VLine是垂直线，HLine是水平线
	line->setFrameShadow(QFrame::Sunken); // 设置阴影效果
	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(view1);
	layout->addWidget(line);
	layout->addWidget(view2);

	// 创建一个中央部件并设置布局
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(layout);

	// 使视图适应图像大小
	//view->fitInView(item, Qt::KeepAspectRatio);

	// 设置 QGraphicsView 为中心窗口部件
	setCentralWidget(centralWidget);
}

//创建的新的工程文件
void StudyCorr::CreateNewProject()
{
	// 使用 QFileDialog 选择文件夹路径
	QString folder = QFileDialog::getExistingDirectory(this, "选择新建的项目文件夹");

	if (!folder.isEmpty()) {
		QDir dir(folder);

		// 生成新的工程文件夹路径
		QString projectFolder = folder + "/NewProject"; // 可以修改为用户输入的名称
		if (dir.mkpath(projectFolder)) {
			// 在新建文件夹中创建一个 .compj 文件
			QString compjFile = projectFolder + "/project.compj";
			QFile file(compjFile);

			if (file.open(QIODevice::WriteOnly)) {
				// 创建一个基本的 JSON 对象保存项目文件信息
				QJsonObject projectJson;
				projectJson["projectName"] = "NewProject";
				projectJson["projectPath"] = projectFolder;
				projectJson["createdDate"] = QDateTime::currentDateTime().toString();

				// 写入 JSON 数据到 .compj 文件
				QJsonDocument doc(projectJson);
				file.write(doc.toJson());
				file.close();

				QMessageBox::information(this, "成功", "新建项目成功！");
			}
			else {
				QMessageBox::critical(this, "失败", "创建项目文件失败！");
			}
		}
		else {
			QMessageBox::critical(this, "失败", "创建新项目文件夹失败！");
		}
	}
}

//打开工程文件
void StudyCorr::OpenExistingProject()
{
	// 使用 QFileDialog 选择一个 .compj 文件
	QString filePath = QFileDialog::getOpenFileName(this, "选择项目文件", "", "项目文件 (*.compj)");

	if (!filePath.isEmpty()) {
		QFile file(filePath);
		if (file.open(QIODevice::ReadOnly)) {
			// 读取文件内容并解析为 JSON
			QByteArray data = file.readAll();
			QJsonDocument doc = QJsonDocument::fromJson(data);
			QJsonObject projectJson = doc.object();

			QString projectName = projectJson["projectName"].toString();
			QString projectPath = projectJson["projectPath"].toString();
			QString createdDate = projectJson["createdDate"].toString();

			// 可以根据读取的项目数据进行其他操作
			QMessageBox::information(this, "项目已打开",
				"项目名称: " + projectName + "\n路径: " + projectPath + "\n创建日期: " + createdDate);
		}
		else {
			QMessageBox::critical(this, "错误", "无法打开项目文件！");
		}
	}
}

//****************************************************标定****************************************************//
//点击标定按钮
void StudyCorr::CalibrationButtonClicked(int CalibrationIndex)
{
	qDebug() << "Button clicked, opening dialog";

	CalibrationDialog* dialog = new CalibrationDialog(this, CalibrationIndex);

	// 点击加载文件
	connect(dialog->TableWidget->horizontalHeader(), &QHeaderView::sectionClicked, dialog, &CalibrationDialog::OnHeaderClicked);

	// Ensure dialog gets deleted when closed
	dialog->setAttribute(Qt::WA_DeleteOnClose);

	// Connect accepted signal to onOkClicked slot
	connect(dialog, &CalibrationDialog::accepted, this, &StudyCorr::CalibrationOKButtonClicked);

	//要获取dialog的信息，需要在主窗口设置一个变量储存，要不然dialog关闭信息就消失了
	//非常重要，坑了我两天。。。。。。。
	calibrationDialog = dialog;

	// Show the dialog
	dialog->show();

	// 手动更新主界面->你到底更不更新啊，还闪退，玩不起别玩
	this->update();
}

//点击CalibrationDialog中的确定按钮
void StudyCorr::CalibrationOKButtonClicked()
{
	qDebug() << "Dialog accepted, running onOkClicked";
	if (calibrationDialog) {
		CalibrationFileNames = calibrationDialog->GetLeftFileNames();  // 获取文件名列表

		// 添加顶层项
		QTreeWidgetItem* CalibrationItem = new QTreeWidgetItem(QStringList(calibrationDialog->NameLineEdit->text()));
		TreeWidget1->addTopLevelItem(CalibrationItem);
		CalibrationItem->setTextAlignment(0, Qt::AlignLeft);// 设置列文本居中

		for (int row = 0; row < calibrationDialog->TableWidget->rowCount(); ++row) {
			QTableWidgetItem* item = calibrationDialog->TableWidget->item(row, 1);
			if (item) {
				// 获取文件名
				QString FileName = item->text();

				// 添加子节点
				QTreeWidgetItem* CalibrationImage = new QTreeWidgetItem(QStringList(FileName));
				CalibrationItem->addChild(CalibrationImage);
			

				// 设置子项文本居中
				CalibrationImage->setTextAlignment(0, Qt::AlignLeft);
			}
		}
		TreeWidget1->update();  // 强制刷新 QTreeWidget
	}
}

//创建标定工具栏
void StudyCorr::CalibrationToolBar()
{
	//添加标定工具栏
	QToolBar* CalibrationToolBar = new QToolBar(this);//工具栏默认不放在树中
	//addToolBar(ToolBar)
	addToolBar(Qt::TopToolBarArea, CalibrationToolBar);//更改停靠区域
	//设置允许停靠区域
	CalibrationToolBar->setAllowedAreas(Qt::TopToolBarArea);
	//CalibrationToolBar->setMovable(false);

	//工具栏加工具项
	QComboBox* ComboBox = new QComboBox(this);//添加下拉框
	// 向下拉框中添加选项
	ComboBox->addItem("chess");
	ComboBox->addItem("circle");
	ComboBox->addItem("code");
	// 将下拉框添加到工具栏
	CalibrationToolBar->addWidget(ComboBox);
	CalibrationToolBar->addSeparator();

	//连接信号和槽
	// 点击计算按钮隐藏标定工具栏
	connect(ComputeButton, &QAction::triggered, [=]() {
		CalibrationToolBar->hide(); });
	// 连接下拉框选择变化的信号
	connect(ComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
		switch (index) {
		case 0:
			qDebug() << "Executing command for Option 1";
			ChessToolBar();
			break;
		case 1:
			qDebug() << "Executing command for Option 2";
			chessToolBar->hide();
			break;
		case 2:
			qDebug() << "Executing command for Option 3";
			chessToolBar->hide();
			break;
		default:
			qDebug() << "Unknown option selected.";
			break;
		} });
}

//创建棋盘格标定工具栏
void StudyCorr::ChessToolBar()
{
	//添加棋盘格标定工具栏
	chessToolBar = new QToolBar(this);//工具栏默认不放在树中
	//addToolBar(ToolBar)
	addToolBar(Qt::TopToolBarArea, chessToolBar);//更改停靠区域
	//设置允许停靠区域
	chessToolBar->setAllowedAreas(Qt::TopToolBarArea);
	//工具栏加工具项
	QComboBox* ComboBox = new QComboBox(this);//添加下拉框
	//QLabel* ComBoxLable = new QLabel("相机个数");
	ComboBox->setToolTip("相机个数");

	// 向下拉框中添加选项 
	ComboBox->addItem("2");
	ComboBox->addItem("3");
	ComboBox->addItem("4");
	// 将下拉框添加到工具栏
	chessToolBar->addWidget(ComboBox);
	chessToolBar->addSeparator();

	// 创建选择棋盘格边长的数字输入框
	QSpinBox* boardSizeSpinBox = new QSpinBox(this);
	boardSizeSpinBox->setMinimum(1); // 设置最小值为1（棋盘格边长不能小于1）
	boardSizeSpinBox->setMaximum(100000); // 设置最大值
	boardSizeSpinBox->setValue(3); // 默认边长为3
	boardSizeSpinBox->setSuffix("mm"); // 显示单位
	boardSizeSpinBox->setToolTip("输入棋盘格的边长（单位：毫米）");
	boardSizeSpinBox->setFixedWidth(110);
	chessToolBar->addWidget(boardSizeSpinBox);
	chessToolBar->addSeparator();

	// 创建选择棋盘格的行数和列数
	QSpinBox* rowsSpinBox = new QSpinBox(this);
	rowsSpinBox->setMinimum(1);
	rowsSpinBox->setMaximum(200);
	rowsSpinBox->setValue(11); // 默认行数
	rowsSpinBox->setSuffix("行");
	rowsSpinBox->setToolTip("输入棋盘格的角点行数");
	rowsSpinBox->setFixedWidth(100);

	QSpinBox* colsSpinBox = new QSpinBox(this);
	colsSpinBox->setMinimum(1);
	colsSpinBox->setMaximum(200);
	colsSpinBox->setValue(8); // 默认列数
	colsSpinBox->setSuffix("列");
	colsSpinBox->setToolTip("输入棋盘格的角点列数");
	colsSpinBox->setFixedWidth(100);

	// 将行数和列数添加到工具栏
	chessToolBar->addWidget(rowsSpinBox);
	chessToolBar->addSeparator();
	chessToolBar->addWidget(colsSpinBox);
	chessToolBar->addSeparator();

	QAction* StartCalibrationButton = chessToolBar->addAction("开始标定");
	StartCalibrationButton->setIcon(QIcon("icon/9.png"));
	connect(StartCalibrationButton, &QAction::triggered, []() {

		});
}

//****************************************************计算****************************************************//
//点击计算按钮
void StudyCorr::ComputeButtonClicked(int ComputeIndex)
{
	qDebug() << "Button clicked, opening dialog";

	ComputeDialog* dialog = new ComputeDialog(this, ComputeIndex);

	// 点击加载文件
	connect(dialog->TableWidget->horizontalHeader(), &QHeaderView::sectionClicked, dialog, &ComputeDialog::OnHeaderClicked);

	// Ensure dialog gets deleted when closed
	dialog->setAttribute(Qt::WA_DeleteOnClose);

	// Connect accepted signal to onOkClicked slot
	connect(dialog, &ComputeDialog::accepted, this, &StudyCorr::ComputeOKButtonClicked);

	computeDialog = dialog;

	// Show the dialog
	dialog->show();

	// 手动更新主界面->你到底更不更新啊，还闪退，玩不起别玩
	this->update();
}

//点击ComputeDialog中的确定按钮
void StudyCorr::ComputeOKButtonClicked()
{
	qDebug() << "Dialog accepted, running onOkClicked";
	if (computeDialog) {
		ComputeFileNames = computeDialog->GetLeftFileNames();  // 获取文件名列表

		// 添加顶层项
		QTreeWidgetItem* ComputeItem = new QTreeWidgetItem(QStringList(computeDialog->NameLineEdit->text()));
		TreeWidget2->addTopLevelItem(ComputeItem);
		ComputeItem->setTextAlignment(0, Qt::AlignLeft);// 设置列文本居中

		for (int row = 0; row < computeDialog->TableWidget->rowCount(); ++row) {
			QTableWidgetItem* item = computeDialog->TableWidget->item(row, 1);
			if (item) {
				// 获取文件名
				QString FileName = item->text();

				// 添加子节点
				QTreeWidgetItem* ComputeImage = new QTreeWidgetItem(QStringList(FileName));
				ComputeItem->addChild(ComputeImage);


				// 设置子项文本居中
				ComputeImage->setTextAlignment(0, Qt::AlignLeft);
			}
		}
		TreeWidget1->update();  // 强制刷新 QTreeWidget
	}
}

//创建计算工具栏
void StudyCorr::ComputeToolBar()
{
	//添加标定工具栏
	QToolBar* ComputeToolBar = new QToolBar(this);//工具栏默认不放在树中
	//addToolBar(ToolBar)
	addToolBar(Qt::TopToolBarArea, ComputeToolBar);//更改停靠区域
	//设置允许停靠区域
	ComputeToolBar->setAllowedAreas(Qt::TopToolBarArea);

	//工具栏加工具项

	connect(CalibrationButton, &QAction::triggered, [=]() {
		ComputeToolBar->hide(); }
	);
	//连接信号和槽
	// 连接下拉框选择变化的信号
	//connect(ComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MyWindow::onComboBoxChanged);
}


