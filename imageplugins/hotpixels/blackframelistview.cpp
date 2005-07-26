/* ============================================================
* File  : imageeffect_hotpixels.cpp
* Author: Unai Garro <ugarro at users dot sourceforge dot net>
* Date  : 2005-07-05
* Description :  a ListView to display black frames
*
* Copyright 2005 by Unai Garro
*
*
* This program is free software; you can redistribute it
* and/or modify it under the terms of the GNU General
* Public License as published by the Free Software Foundation;
* either version 2, or (at your option)
* any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* ============================================================ */

#include "blackframelistview.h"
#include <qpainter.h>
#include "blackframeparser.h"

#define THUMB_WIDTH 150
BlackFrameListViewItem::BlackFrameListViewItem(BlackFrameListView* parent,KURL url):KListViewItem(parent),QObject(parent)
{
	mParser.parseBlackFrame(url);
	
	connect(&mParser,SIGNAL(parsed(QValueList<HotPixel>)),this,SLOT(slotParsed(QValueList<HotPixel>)));
}

QString BlackFrameListViewItem::text(int column)const
{
	switch (column)
	{
		case 0:
			return (""); //First column includes the pixmap
			break;
		case 1:
			return (QString("%1x%2").arg(mImageSize.width()).arg(mImageSize.height())); 
			break;
		case 2:
			return ("yes"); //Temporarily
			break;
	}
	return("foo");
}

void BlackFrameListViewItem::paintCell(QPainter* p,const QColorGroup& cg,int column,int width,int align)
{
	//Let the normal listview item draw it all for now
	QListViewItem::paintCell(p,cg,column,width,align);

}

BlackFrameListView::BlackFrameListView(QWidget* parent):KListView(parent)
{
}

void BlackFrameListViewItem::slotParsed(QValueList<HotPixel> hotPixels)
{
	mHotPixels=hotPixels;
	mImage=mParser.image();
	mImageSize=mImage.size();
	mThumb=thumb(QSize(THUMB_WIDTH,THUMB_WIDTH/3*2));
	setPixmap(0,mThumb);
}

QPixmap BlackFrameListViewItem::thumb(QSize size)
{
	QPixmap thumb;
	
	//First scale it down to the size
	thumb= mImage.smoothScale(size,QImage::ScaleMin);
	
	//And draw the hot pixel positions on the thumb
	QPainter p(&thumb);

		//Take scaling into account
	float xRatio, yRatio;
	float hpThumbX,hpThumbY;
	QRect hpRect;
	
	xRatio=(float)size.width()/(float)mImage.width();
	yRatio=(float)size.height()/(float)mImage.height();
	
		//Draw hot pixels one by one
	QValueList <HotPixel>::Iterator it;	
	for (it=mHotPixels.begin() ; it!=mHotPixels.end() ; ++it)
	{
		hpRect=(*it).rect;
		hpThumbX=(hpRect.x()+hpRect.width()/2)*xRatio;
		hpThumbY=(hpRect.y()+hpRect.height()/2)*yRatio;
		
		p.setPen(QPen(Qt::black));
		p.drawLine(hpThumbX,hpThumbY-1,hpThumbX,hpThumbY+1);
		p.drawLine(hpThumbX-1,hpThumbY,hpThumbX+1,hpThumbY);
		p.setPen(QPen(Qt::white));
		p.drawPoint(hpThumbX-1,hpThumbY-1);
		p.drawPoint(hpThumbX+1,hpThumbY+1);
		p.drawPoint(hpThumbX-1,hpThumbY+1);
		p.drawPoint(hpThumbX+1,hpThumbY-1);
	}
	return thumb;
	
}

int BlackFrameListViewItem::width(const QFontMetrics& fm,const QListView* lv,int c)const
{
	if (c==0) return THUMB_WIDTH;
	else return QListViewItem::width(fm,lv,c);
}
#include "blackframelistview.moc"