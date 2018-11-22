#ifndef PAGESETUP_H
#define PAGESETUP_H

#include <QDialog>
#include <QTimer>

namespace Ui
{
	class Dialog;
}

class PageSetup : public QDialog
{
	Q_OBJECT
public:
	explicit PageSetup(QWidget *parent = 0);
	~PageSetup();

	void	load_data();
	QSize	pageSize;
	QRect	pageBorders;
	QRect	graphBorders;
	QSize	gridStep;
    float   zoom;

private slots:
	void	update();
    void    on_zoomSlider_valueChanged(int value);
    void    on_pageZoom_editingFinished();

signals:
	void	onApply();
    void    onZoomChanged(float scale);

private:
	Ui::Dialog *ui;
};

#endif // PAGESETUP_H
