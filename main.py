import os
import random
from svglib.svglib import svg2rlg
from reportlab.graphics import renderPDF
from PyPDF2 import PdfFileMerger

root = os.path.dirname(os.path.abspath(__file__))


def svg(contours):
    global root
    merger = PdfFileMerger()
    i = 0
    code = ''
    curr_z = contours[0][2]
    while i < len(contours):
        if curr_z != contours[i][2] or i == len(contours) - 1:
            code += '<text x = "-149.5" y="-141" font-size="12" font-weight="bold" font-family="Avenir, Helvetica, sans-serif">z={}</text>'.format(curr_z)
            code += '</svg>'
            path = root+'/files/'+str(curr_z)
            f = open(path+'.svg', 'w')
            f.write(code)
            f.close()
            dwg = svg2rlg(path+'.svg')
            renderPDF.drawToFile(dwg, path+'.pdf')
            merger.append(path+'.pdf')
            os.remove(path+'.svg')
            os.remove(path+'.pdf')

        if curr_z != contours[i][2] or i == 0:
            code = '<svg xmlns="http://www.w3.org/2000/svg" width="300" height="300" version="1.1" viewBox="-150 -150 300 300">\n'
            curr_z = contours[i][2]

        color = '#{:02x}{:02x}{:02x}'.format(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        contour = contours[i]
        del contour[2::3]
        contour = [(contour[j], contour[j+1]) for j in range(0, len(contour), 2)]
        code += '<polyline points="{}" stroke="{}" stroke-width="1" fill="none"/>\n'.format(' '.join([str(contour[j][0])+','+str(contour[j][1]) for j in range(len(contour))]), color)
        for point in contour:
            code += '<circle cx="{}" cy="{}" r="0.6" fill="{}" stroke="black" stroke-width="0.2"/>'.format(point[0], point[1], color)
            code += '<text x="{}" y="{}" text-anchor="middle" font-size="2px">{}</text>\n'.format(point[0], point[1]-0.7, str(point[0])+' '+str(point[1]))
        i += 1
    merger.write(root+'/files/model.pdf')
    os.system('open '+root+'/files/model.pdf')


if __name__ == '__main__':
    f = open(root + '/files/model.txt')
    data = f.readlines()
    f.close()
    data = " ".join(data)
    data = data.split('\n')
    del data[len(data)-1]

    for i in range(len(data)):
        data[i] = list(map(float, data[i].split()))
    svg(data)