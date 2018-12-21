import os
import random
from svglib.svglib import svg2rlg
from reportlab.graphics import renderPDF
from PyPDF2 import PdfFileMerger

root = os.path.dirname(os.path.abspath(__file__))


def svg(contoures):
    global root
    merger = PdfFileMerger()
    i = 0
    code = ''
    curr_z = contoures[0][2]
    while i < len(contoures):
        if curr_z != contoures[i][2] or i == len(contoures) - 1:
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
        if curr_z != contoures[i][2] or i == 0:
            code = '<svg xmlns="http://www.w3.org/2000/svg" width="300" height="300" version="1.1" viewBox="-150 -150 300 300">\n'
            curr_z = contoures[i][2]
        color = "#{:02x}{:02x}{:02x}".format(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        for j in range(0, len(contoures[i])-3, 3):
            code += '<line x1="{}" y1="{}" x2="{}" y2="{}" stroke="{}" stroke-width="0.5"/>'.format(contoures[i][j], contoures[i][j+1], contoures[i][j+3], contoures[i][j+4], color)
            code += '<circle cx="{}" cy="{}" r="0.25" fill="{}" stroke="black" stroke-width="0.2"/>'.format(contoures[i][j], contoures[i][j+1], color)
            code += '<circle cx="{}" cy="{}" r="0.25" fill="{}"  stroke="black" stroke-width="0.2"/>\n'.format(contoures[i][j+3], contoures[i][j+4], color)
            code += '<text x="{}" y="{}" text-anchor="middle" font-size="1px">{} {}</text>'.format(contoures[i][j], contoures[i][j+1], contoures[i][j], contoures[i][j+1])
            code += '<text x="{}" y="{}" text-anchor="middle" font-size="1px">{} {}</text>'.format(contoures[i][j+3], contoures[i][j+4], contoures[i][j+3], contoures[i][j+4])
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