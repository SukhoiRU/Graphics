#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

class ColorButton : public QPushButton
{
	Q_OBJECT

public:
	ColorButton(QWidget *parent);
	~ColorButton();

	virtual void paintEvent(QPaintEvent * event) Q_DECL_OVERRIDE;
	virtual void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

	void	setColor(QColor& c);
	void	setDiffColor();
	bool	getColor(QColor& c);
private:
	QColor	m_color;
	bool	m_bDiff;
};

#endif // COLORBUTTON_H
