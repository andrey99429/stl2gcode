import os
import random
from svglib.svglib import svg2rlg
from reportlab.graphics import renderPDF
from PyPDF2 import PdfFileMerger

root = os.path.dirname(os.path.abspath(__file__))


def svg(contours):
    global root

    svgs = {}
    for i in range(len(contours)):
        curr_z = contours[i][2]
        color = '#{:02x}{:02x}{:02x}'.format(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        width = 1
        contour = contours[i]
        del contour[2::3]
        contour = [(contour[j], contour[j+1]) for j in range(0, len(contour), 2)]
        code = ''
        if len(contour) == 2:
            width = 0.25
            color = '#b3b3b3'
        code += '<polyline points="{}" stroke="{}" stroke-width="{}" fill="none"/>'.format(' '.join([str(contour[j][0])+','+str(contour[j][1]) for j in range(len(contour))]), color, width)
        for point in contour:
            code += '<circle cx="{}" cy="{}" r="{}" fill="orange" stroke="black" stroke-width="0.25"/>'.format(point[0], point[1], width)
            #code += '<text x="{}" y="{}" text-anchor="middle" font-size="3px">{}</text>\n'.format(point[0], point[1]-1.25, str(point[0])+' '+str(point[1]))
        code += '\n'
        if curr_z not in svgs:
            svgs[curr_z] = ''
        svgs[curr_z] += code

    merger = PdfFileMerger()
    for curr_z, code in svgs.items():
        svg = '<svg xmlns="http://www.w3.org/2000/svg" width="300" height="300" version="1.1" viewBox="-150 -150 300 300">\n'
        svg += code
        svg += '<text x = "-149.5" y="-141" font-size="12" font-weight="bold" font-family="Avenir, Helvetica, sans-serif">z={}</text>\n'.format(curr_z)
        svg += '</svg>'
        path = root+'/files/'+str(curr_z)
        f = open(path+'.svg', 'w')
        f.write(svg)
        f.close()
        dwg = svg2rlg(path+'.svg')
        renderPDF.drawToFile(dwg, path+'.pdf')
        merger.append(path+'.pdf')
        #os.remove(path+'.svg')
        os.remove(path+'.pdf')

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