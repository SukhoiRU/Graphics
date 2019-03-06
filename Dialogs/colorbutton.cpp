#include "stdafx.h"
#include "colorbutton.h"

ColorButton::ColorButton(QWidget *parent)
	: QPushButton(parent)
{
}

ColorButton::~ColorButton()
{

}

void	ColorButton::setColor(QColor& c)
{
	m_color	= c;
	m_bDiff	= false;
}

void	ColorButton::setDiffColor()
{
	m_bDiff	= true;
}

bool	ColorButton::getColor(QColor& c)
{
	c	= m_color;
	return !m_bDiff;
}

void	ColorButton::paintEvent(QPaintEvent * event)
{
	QPainter	painter(this);
	if(m_bDiff)
	{
		//Разноцветная кнопка
		QRect	rc	= event->rect();
		QLinearGradient g(rc.topLeft(), rc.bottomRight());
		g.setColorAt(0, Qt::red);
		g.setColorAt(0.5, Qt::blue);
		g.setColorAt(1, Qt::green);
		QBrush	brush(g);
		painter.fillRect(event->rect(), brush);
	}
	else
	{
		painter.fillRect(event->rect(), QBrush(m_color));
	}
	event->accept();
}

void ColorButton::mousePressEvent(QMouseEvent* /*e*/)
{
	QColorDialog	dlg(m_color, this);
#ifdef __linux__
    dlg.setOption(QColorDialog::DontUseNativeDialog);
#endif
	if(dlg.exec())
	{
		m_color	= dlg.currentColor();
		m_bDiff	= false;
	}
}
