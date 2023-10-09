#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtRhythmicaLyrics.h"

#include "RLInterface.h"

QT_BEGIN_NAMESPACE
namespace Ui { class QtRhythmicaLyricsClass; };
QT_END_NAMESPACE

class QtRhythmicaLyrics : public QMainWindow {
	Q_OBJECT

public:
	QtRhythmicaLyrics(QWidget* parent = nullptr);
	~QtRhythmicaLyrics();

private:
	Ui::QtRhythmicaLyricsClass* ui;

	// music player
	void PlayerInit();
	bool sldCurrPressed = false;
	bool sldSpeedPressed = false;
	void SetCurrLab(int cur);
	QString GetTime(int t);
	void UpdateVolTip();
	void UpdateSpdTip();
	void ResetPlayer();
	//////////////////////////////////////////////////////////////////////////

protected:
	void dragEnterEvent(QDragEnterEvent* event); // 拖动进入事件
	void dropEvent(QDropEvent* event);           // 放下事件

};
