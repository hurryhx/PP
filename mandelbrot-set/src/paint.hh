

#ifndef __PAINT__
#define __PAINT__
#include<cassert>
#include"image.hh"

class Paint{
	public:
		int m_size;
		ColorRGB *color;

		Paint(){}
		Paint(int size){
			ColorRGB col(1, 1, 1);
			int drgb[][3] = {
				{-1, 0, 0},
				{0, -1, 0},
				{1, 0, 0},
				{0, 0, -1},
				{0, 1, 0},
				{-1, 0, 0},
				{0, -1, 0}
			};

			int nphase = sizeof(drgb) / sizeof(int) / 3;
			m_size = size / nphase * nphase; //cut the tail
			if (m_size != size) m_size += nphase;
			color = new ColorRGB[m_size];
			int psize = m_size / nphase;
			double delta = 1.0 / psize;
			for (int i = 0, pos = 0, phase = 0; i < m_size; i+=psize, phase++){
				int *udelta = drgb[phase];
				for (int j = 0; j < psize; j++, pos++){
					col.r += udelta[0] * delta;
					col.g += udelta[1] * delta;
					col.b += udelta[2] * delta;
					assert(pos < m_size);
					color[pos] = col;
				}
			}
		}
		virtual ColorRGB Color(int seed){return color[seed%m_size];}
		virtual ~Paint(){delete[] color;}
};

class GrayPaint : public Paint{
	public:
		GrayPaint(int size){
			size++;
			m_size = size;
			color = new ColorRGB[size];
			for(int i = 0; i < size; i++)
			{
				double col = (size-i-1)/(double)size;
				color[i] = ColorRGB(col, col, col);
			}
		}
};
#endif
