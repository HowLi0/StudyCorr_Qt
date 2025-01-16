#include "ComputeDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include<QLabel>
#include<QFileDialog>
#include<QTreeWidgetItem>
#include<QStringList>

int ComputeDialog::ComputeIndex = 1;

//****************************************************ComputeDialog****************************************************//

ComputeDialog::ComputeDialog(QWidget* parent, int ComputeIndex)
    : QDialog(parent)
{
    this->setWindowTitle(QString("计算对话框"));
    this->setFixedSize(600, 400);
    this->ComputeIndex = ComputeIndex;
    SetupUi();
}

ComputeDialog::~ComputeDialog()
{}

void ComputeDialog::SetupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Create top widgets
    CameraComboBox = new QComboBox(this);
    CameraComboBox->addItem("2");
    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addWidget(new QLabel("Number of cameras:", this));
    topLayout->addWidget(CameraComboBox);
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    NameLineEdit = new QLineEdit(this);
    QString name = QString("Compute%1").arg(ComputeIndex);
    NameLineEdit->setText(name);
    QHBoxLayout* nameLayout = new QHBoxLayout;
    nameLayout->addWidget(new QLabel("name:", this));
    nameLayout->addWidget(NameLineEdit);
    mainLayout->addLayout(nameLayout);

    // Create table widget
    TableWidget = new QTableWidget();
    TableWidget->setColumnCount(3);
    TableWidget->setColumnWidth(0, 60); // 设置第一列宽度为100像素
    TableWidget->setColumnWidth(1, 270); // 设置第二列宽度为150像素
    TableWidget->setColumnWidth(2, 270); // 设置第三列宽度为120像素
    QStringList headers = { "State", "Camera Position1(click here to set all)", "Camera Position2(click here to set all)" };
    TableWidget->setHorizontalHeaderLabels(headers);
    TableWidget->horizontalHeader()->setStretchLastSection(true);

    mainLayout->addWidget(TableWidget);

    // Create buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    OkButton = new QPushButton("确定", this);
    CancelButton = new QPushButton("取消", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(OkButton);
    buttonLayout->addWidget(CancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(OkButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(CancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void ComputeDialog::OnHeaderClicked(int section)
{

    if (section == 1 || section == 2)
    {
        FileNames = QFileDialog::getOpenFileNames(this, tr("Select Files"), "", tr("Images (*.bmp *.png *.jpg)"));
        if (!FileNames.isEmpty())
        {
            //设置行数显示文件
            TableWidget->setRowCount(FileNames.count());
            for (int i = 0; i < FileNames.count(); ++i)
            {
                //在*******************dialog***************中显示
                //提取文件名
                QFileInfo fileInfo(FileNames[i]);
                QString fileName = fileInfo.fileName();
                //设置文件个数和√
                QTableWidgetItem* stateItem = new QTableWidgetItem;
                stateItem->setCheckState(Qt::Checked);
                TableWidget->setItem(i, 0, stateItem);
                // tableWidget->setItem(i, section, new QTableWidgetItem(fileNames[0]));
                TableWidget->setItem(i, section, new QTableWidgetItem(fileName));
            }
        }

    }
}

QStringList ComputeDialog::GetLeftFileNames()const {
    QStringList fileNames;
    for (int row = 0; row < TableWidget->rowCount(); ++row) {
        QTableWidgetItem* item = TableWidget->item(row, 0); // 左相机文件路径在第二列
        if (item) {
            fileNames.append(item->text());
        }
    }
    return fileNames;
}

QStringList ComputeDialog::GetRightFileNames() const {
    QStringList fileNames;
    for (int row = 0; row < TableWidget->rowCount(); ++row) {
        QTableWidgetItem* item = TableWidget->item(row, 1); // 右相机文件路径在第三列
        if (item) {
            fileNames.append(item->text());
        }
    }
    return fileNames;
}
