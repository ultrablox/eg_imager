
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>

struct img_processor_base
{
	img_processor_base(const QString & odir, QImage * pix_logo)
	:outDir(odir), pPixLogo(pix_logo)
	{}

	void center_draw(QImage & dest, const QImage & img)
	{
		QPainter painter(&dest);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

		QPoint center(dest.width() / 2, dest.height() / 2);

		float h_k = (float)dest.width() / img.width(),
			v_k = (float)dest.height() / img.height();

		float scale_k = std::min(v_k, h_k);

		float w = img.width() * scale_k,
			h = img.height() * scale_k;
		QRectF dest_rect(center.x() - (w / 2), center.y() - (h / 2), w, h);

		painter.drawImage(dest_rect, img);
	}
	
	const QString outDir;
	QImage * pPixLogo;
};


struct large_with_logo : public img_processor_base
{
	large_with_logo(QImage * wpix)
	:img_processor_base("1200x1080 L", wpix)
	{}

	void execute(QImage & dest, const QImage & src_img, const QString & base_name)
	{
		dest = QImage(1200, 1080, QImage::Format::Format_RGB32);

		{
			QPainter painter(&dest);
			painter.setBrush(QBrush(QColor(255, 255, 255, 255)));
			painter.setPen(Qt::PenStyle::NoPen);
			painter.drawRect(QRect(0, 0, dest.width(), dest.height()));
		}

		center_draw(dest, src_img);
		center_draw(dest, *pPixLogo);

		dest.save(outDir + "/" + base_name + "L" + ".png", "PNG");
	}
};

struct small_nologo : public img_processor_base
{
	small_nologo(QImage * wpix)
	:img_processor_base("150x135 S", 0)
	{}

	void execute(QImage & dest, const QImage & src_img, const QString & base_name)
	{
		dest = QImage(150, 135, QImage::Format::Format_RGB32);

		{
			QPainter painter(&dest);
			painter.setBrush(QBrush(QColor(255, 255, 255, 255)));
			painter.setPen(Qt::PenStyle::NoPen);
			painter.drawRect(QRect(0, 0, dest.width(), dest.height()));
		}

		center_draw(dest, src_img);
		dest.save(outDir + "/" + base_name + "S" + ".png", "PNG");
	}
};

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	std::string fname(argv[1]);
	QString src_name(QString::fromStdString(fname));
	QFileInfo fi(src_name);


	QImage img_logo("watermark.png"),
			img_src(src_name);

	//1
	img_src.save("Nologo Origin/" + fi.baseName() + "L" + ".png", "PNG");
	
	//2
	QImage with_logo;
	auto large_logo = new large_with_logo(&img_logo);
	large_logo->execute(with_logo, img_src, fi.baseName());

	//3
	{
		QImage dest_small(300, 270, QImage::Format::Format_RGB32);

		QPainter painter(&dest_small);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
		
		painter.setBrush(QBrush(QColor(255, 255, 255, 255)));
		painter.setPen(Qt::PenStyle::NoPen);
		painter.drawRect(QRect(0, 0, dest_small.width(), dest_small.height()));

		painter.drawImage(QRectF(0, 0, dest_small.width(), dest_small.height()), with_logo);


		dest_small.save("300x270 M/" + fi.baseName() + "M" + ".png", "PNG");
	}

	auto small_nlogo = new small_nologo(&img_logo);
	QImage no_logo;
	small_nlogo->execute(no_logo, img_src, fi.baseName());
	

	delete large_logo;
	delete small_nlogo;


	//return a.exec();
	return 0;
}
