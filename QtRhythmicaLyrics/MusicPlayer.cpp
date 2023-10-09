#include "QtRhythmicaLyrics.h"

//using namespace Player;
void QtRhythmicaLyrics::PlayerInit() {
	Player::PlayerInit();
	ResetPlayer();


	//输入文件
	connect(Player::player, &QMediaPlayer::mediaStatusChanged,
		[&](QMediaPlayer::MediaStatus state) {
			if (state == QMediaPlayer::MediaStatus::LoadedMedia) {
				//初始化dur相关
				auto dur = Player::player->duration();
				ui->sldCurr->setMaximum(dur);
				ui->sldCurr->setTickInterval(dur / 10);
				SetCurrLab(0);
				//TODO: fileName update
			}
		});

	//播放，停止，暂停
	//InitBtnIcon();
	connect(ui->btnPlay, &QPushButton::clicked,
		Player::player, &QMediaPlayer::play);
	connect(ui->btnStop, &QPushButton::clicked,
		Player::player, &QMediaPlayer::stop);
	connect(ui->btnPause, &QPushButton::clicked,
		Player::player, &QMediaPlayer::pause);
	//快退，快进
	connect(ui->btnBack, &QPushButton::clicked,
		[&] {
			int curr = Player::player->position();
			curr -= Player::bwdTime * 1000;
			if (curr < 0) {
				curr = 0;
			}
			Player::player->setPosition(curr);
		});
	connect(ui->btnForward, &QPushButton::clicked,
		[&] {
			int curr = Player::player->position();
			curr += Player::fwdTime * 1000;
			if (curr > Player::player->duration()) {
				curr = Player::player->duration();
			}
			Player::player->setPosition(curr);
		});


	//进度条
	connect(Player::player, &QMediaPlayer::positionChanged,
		[&](qint64 currPos) {
			ui->sldCurr->setSliderPosition(currPos);
			//SetCurrPosLab(currPos);
		});
	connect(ui->sldCurr, &QSlider::sliderPressed,
		[&] {
			sldCurrPressed = true;
			if (Player::player->playbackState() == QMediaPlayer::PlaybackState::PlayingState) {
				Player::player->pause();
			}
		});
	connect(ui->sldCurr, &QSlider::sliderReleased,
		[&] {
			sldCurrPressed = false;
			qint64 curr = ui->sldCurr->value();
			Player::player->setPosition(curr);
			if (Player::player->playbackState() == QMediaPlayer::PlaybackState::PausedState) {
				Player::player->play();
			}
		});

	connect(ui->sldCurr, &QSlider::valueChanged,
		[&](qint64 curr) {
			SetCurrLab(curr);
		});

	//音量
	connect(ui->sldVolume, &QSlider::valueChanged,
		[&](int vol) {
			float volf = (float)vol / 100;
			Player::audioOutput->setVolume(volf);
			UpdateVolTip();
		});

	//播放速度
	connect(ui->sldSpeed, &QSlider::valueChanged,
		[&](int val) {
			val += 20;
			Player::player->setPlaybackRate((qreal)val / 100);
			UpdateSpdTip();
		});
	connect(ui->sldSpeed, &QSlider::sliderPressed,
		[&] {
			sldSpeedPressed = true;
			if (Player::player->playbackState() == QMediaPlayer::PlaybackState::PlayingState) {
				Player::player->pause();
			}
		});
	connect(ui->sldSpeed, &QSlider::sliderReleased,
		[&] {
			sldSpeedPressed = false;
			if (Player::player->playbackState() == QMediaPlayer::PlaybackState::PausedState) {
				Player::player->play();
			}
		});
}


void QtRhythmicaLyrics::SetCurrLab(int cur) {
	QString strCurr = GetTime(cur);
	ui->labCurr->setText(strCurr + '/' + GetTime(Player::player->duration()));
}

QString QtRhythmicaLyrics::GetTime(int t) {
	QTime tTmp;
	tTmp.setHMS(0, 0, 0);
	tTmp = tTmp.addMSecs(t);
	QString str = tTmp.toString("[mm:ss:zzz");
	str[9] = ']';
	return str;
}

void QtRhythmicaLyrics::UpdateVolTip() {
	QString str
		= "音量："
		+ QString::number((double)Player::audioOutput->volume() * 100)
		+ '%';
	ui->sldVolume->setToolTip(str);
}

void QtRhythmicaLyrics::UpdateSpdTip() {
	QString str
		= "播放速度："
		+ QString::number(Player::player->playbackRate() * 100)
		+ "%";
	ui->sldSpeed->setToolTip(str);
}


void QtRhythmicaLyrics::ResetPlayer() {
	ui->sldCurr->setValue(0);
	ui->sldSpeed->setMaximum(80);
	ui->sldSpeed->setValue(80);
	ui->sldVolume->setMaximum(100);
	ui->sldVolume->setValue(50);
	Player::player->setPlaybackRate(1);
	Player::audioOutput->setVolume(0.5);
	UpdateVolTip();
	UpdateSpdTip();
}