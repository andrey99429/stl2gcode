import os
import sys
import plotly.plotly as py
import plotly.offline.offline as py_off
import plotly.graph_objs as go
from svglib.svglib import svg2rlg
from reportlab.graphics import renderPDF
from PyPDF2 import PdfFileMerger

root = os.path.dirname(os.path.abspath(__file__))

def plotly(mode, model, segments):
    # model
    plot_data = []
    mesh = go.Mesh3d(
        x=model[0::3],
        y=model[1::3],
        z=model[2::3],
        i=list(range(0, len(model)//3, 3)),
        j=list(range(1, len(model)//3, 3)),
        k=list(range(2, len(model)//3, 3)),
        showlegend=False,
        opacity=0.9,
        color='#87ceeb'
    )
    mesh.name = 'mesh'
    plot_data.append(mesh)

    # segments
    i = 0
    while i < len(segments):
        v1 = segments[i:i+3]
        v2 = segments[i+3:i+6]
        scatter = go.Scatter3d(
            x = [v1[0], v2[0]],
            y = [v1[1], v2[1]],
            z = [v1[2], v2[2]],
            showlegend=False,
            marker=dict(
                color='black',
                size=3,
            ),
            line=dict(
                color='black',
                width=1,
                dash='dash'
            ),
            name='line '+str(i//6)
        )
        plot_data.append(scatter)
        i += 6

    layout = go.Layout()
    fig = dict(data=plot_data, layout=layout)
    if mode == 'plot':
        py_off.plot(fig, filename=root + '/files/model.html')
    else:
        py.plot(fig, filename='model')


def svg(segments, contoures):
    global root
    '''
    merger = PdfFileMerger()
    i = 0
    code = ''
    curr_z = segments[2]
    while i < len(segments):
        v1 = segments[i:i+3]
        v2 = segments[i+3:i+6]
        if curr_z != v1[2] or i == len(segments) - 1:
            code += '<text x = "-150" y="-140" font-size="12" font-weight="bold" font-family="Avenir, Helvetica, sans-serif">z={}</text>'.format(curr_z)
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

        if curr_z != v1[2] or i == 0:
            code = '<svg xmlns="http://www.w3.org/2000/svg" width="300" height="300" version="1.1" viewBox="-150 -150 300 300">\n'
            curr_z = v1[2]
        code += '<line x1="{}" y1="{}" x2="{}" y2="{}" stroke="blue" stroke-width="1"/>'.format(v1[0], v1[1], v2[0], v2[1])
        code += '<circle cx="{}" cy="{}" r="1" fill="orange" stroke="black" stroke-width="0.25"/>'.format(v1[0], v1[1])
        code += '<circle cx="{}" cy="{}" r="1" fill="orange" stroke="black" stroke-width="0.25"/>\n'.format(v2[0], v2[1])
        i += 6
    merger.write(root+'/files/model_seg.pdf')
    os.system('open '+root+'/files/model_seg.pdf')
    '''
    merger = PdfFileMerger()
    i = 0
    code = ''
    curr_z = contoures[0][2]
    while i < len(contoures):
        if curr_z != contoures[i][2] or i == len(contoures) - 1:
            code += '<text x = "-150" y="-140" font-size="12" font-weight="bold" font-family="Avenir, Helvetica, sans-serif">z={}</text>'.format(curr_z)
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
        for j in range(0, len(contoures[i])-3, 3):
            code += '<line x1="{}" y1="{}" x2="{}" y2="{}" stroke="blue" stroke-width="1"/>'.format(contoures[i][j], contoures[i][j+1], contoures[i][j+3], contoures[i][j+4])
            code += '<circle cx="{}" cy="{}" r="1" fill="orange" stroke="black" stroke-width="0.25"/>'.format(contoures[i][j], contoures[i][j+1])
            code += '<circle cx="{}" cy="{}" r="1" fill="orange" stroke="black" stroke-width="0.25"/>\n'.format(contoures[i][j+3], contoures[i][j+4])
        i += 1
    merger.write(root+'/files/model_cont.pdf')
    os.system('open '+root+'/files/model_cont.pdf')


if __name__ == '__main__':
    if len(sys.argv) == 1:
        sys.exit(1)

    f = open(root + '/files/model.txt')
    data = f.readlines()
    f.close()
    data = " ".join(data)
    data = data.split(';\n')
    data[0] = data[0].replace('\n', '')
    data[1] = data[1].replace('\n', '')
    data[2] = data[2].split('\n')
    del data[2][len(data[2])-1]

    model = list(map(float, data[0].split()))
    segments = list(map(float, data[1].split()))
    for i in range(len(data[2])):
        data[2][i] = list(map(float, data[2][i].split()))
    contoures = data[2]

    mode = sys.argv[1]
    if mode == 'svg':
        svg(segments, contoures)
    else:
        plotly(mode, model, segments)
