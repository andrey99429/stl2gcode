import os
import random
from svglib.svglib import svg2rlg
from reportlab.graphics import renderPDF
from PyPDF2 import PdfFileMerger

root = os.path.dirname(os.path.abspath(__file__))


def svg(shells, infill):
    global root

    svgs = {}
    for i in range(len(shells)):
        curr_z = shells[i][2]
        width = 0.75
        color = '#{:02x}{:02x}{:02x}'.format(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        stroke = 0.25
        shell = shells[i]
        del shell[2::3]
        shell = [(shell[j], shell[j+1]) for j in range(0, len(shell), 2)]
        code = '<polyline points="{}" stroke="{}" stroke-width="{}" fill="none"/>'.format(' '.join([str(shell[j][0])+','+str(shell[j][1]) for j in range(len(shell))]), color, width)
        for point in shell:
            code += '<circle cx="{}" cy="{}" r="{}" fill="orange" stroke="black" stroke-width="{}"/>'.format(point[0], point[1], width, stroke)
        code += '\n'
        if curr_z not in svgs:
            svgs[curr_z] = ''
        svgs[curr_z] += code

    for i in range(len(infill)):
        curr_z = infill[i][2]
        width = 0.1
        color = '#000'
        stroke = 0.1
        line = infill[i]
        del line[2::3]
        line = [(line[j], line[j+1]) for j in range(0, len(line), 2)]
        code = '<polyline points="{}" stroke="{}" stroke-width="{}" fill="none"/>'.format(' '.join([str(line[j][0])+','+str(line[j][1]) for j in range(len(line))]), color, width)
        for point in line:
            code += '<circle cx="{}" cy="{}" r="{}" fill="orange" stroke="black" stroke-width="{}"/>'.format(point[0], point[1], width, stroke)
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
        os.remove(path+'.svg')
        os.remove(path+'.pdf')

    merger.write(root+'/files/model.pdf')
    os.system('open '+root+'/files/model.pdf')


if __name__ == '__main__':
    f = open(root + '/files/model.txt')
    data = f.readlines()
    f.close()

    data = " ".join(data)
    data = data.split(':')
    shells = data[0].split('\n')
    del shells[-1]
    infill = data[1].split('\n')
    del infill[0]
    del infill[-1]

    for i in range(len(shells)):
        shells[i] = list(map(float, shells[i].split()))
    for i in range(len(infill)):
        infill[i] = list(map(float, infill[i].split()))

    svg(shells, infill)
