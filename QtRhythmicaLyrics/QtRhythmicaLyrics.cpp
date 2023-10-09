#include "QtRhythmicaLyrics.h"

#include <QDragEnterEvent>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QMimeData>
#include <QFileInfo>
#include <QMessageBox>




QtRhythmicaLyrics::QtRhythmicaLyrics(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::QtRhythmicaLyricsClass()) {
	ui->setupUi(this);

	setAcceptDrops(true);

	PlayerInit();
	PaintCfg::InitPolyCfg();
}

QtRhythmicaLyrics::~QtRhythmicaLyrics() {
	delete ui;
}

// 拖动进入事件
void QtRhythmicaLyrics::dragEnterEvent(QDragEnterEvent* event) {
	// 仅接受数据中包含URL的事件
	if (event->mimeData()->hasUrls())
		event->acceptProposedAction();
	else event->ignore();
}

// 放下事件
void QtRhythmicaLyrics::dropEvent(QDropEvent* event) {
	const QMimeData* mimeData = event->mimeData();	// 获取MIME数据
	if (mimeData->hasUrls()) {
		QList<QUrl> urlList = mimeData->urls();	// 获取URL列表
		for (QUrl& url : urlList) {
			if (!url.isValid()) continue;
			QString ext = QFileInfo(url.toLocalFile()).suffix();

			if (Player::musicFile.count(ext)) {
				Player::MusicInput(url);
				ResetPlayer();
			}
			else if (Player::lrcFile.count(ext)) {
				Lyrics::LrcInput(url);
			}
			else {
				QMessageBox* msg = new QMessageBox;
				msg->setAttribute(Qt::WA_DeleteOnClose);
				msg->setText("invalid files");
				msg->show();
			}
		}
	}
}

