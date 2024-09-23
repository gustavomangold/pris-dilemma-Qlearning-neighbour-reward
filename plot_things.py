import numpy as np
import pandas as pd
import glob
import io
import matplotlib.pyplot as plt
import matplotlib as mpl
import itertools

def plot_heatmap(x_list, y_list, cooperation_list):
    '''
    Plot the heatmap

    Returns:
    (void)
    '''
    x = np.array(x_list)
    y = np.array(y_list)
    z = np.array(cooperation_list)

    plt.xlabel(r'$b$')
    plt.ylabel(r'$s_r$')

    #plt.gca().invert_yaxis()

    plt.tricontourf(x, y, z, levels = 70, cmap = 'jet_r')
    cbar = plt.colorbar()
    cbar.set_ticks(np.arange(round(min(z), 1), round(max(z), 1), .1))

    plt.savefig('heatmap_coop_versus_prob-diff_and_alpha-share.png', dpi=400, bbox_inches='tight')
    plt.clf()

    return

def plot_separate_column(colnames, colors):
    index = 0
    for column in colnames:
        if column[:2] == 'Qd':
            plt.plot(data[['t']].to_numpy(), data[[column]].to_numpy(),
                color = colors[index % len(colors)], label = r'$Q_{{{}}}$'.format(column[1:]))
            index += 1
    plt.legend(loc = 'best')
    plt.savefig(filename + 'q-table-for-d.png', dpi = 400, bbox_inches='tight')
    plt.close()

    for column in colnames:
        if column[:2] == 'Qc':
            plt.plot(data[['t']].to_numpy(), data[[column]].to_numpy(),
                color = colors[index % len(colors)], label = r'$Q_{{{}}}$'.format(column[1:]))
            index += 1
    plt.legend()
    plt.savefig(filename + 'q-table-for-c.png', dpi = 400, bbox_inches='tight')
    plt.close()

def plot_data_values(filename, data, colnames, colors, identifier: str):
    #plt.title('Prob diffusion = ' + filename.split('P_DIFFUSION')[1][:3] + ' Occupation =' +
    #    filename.split('rho')[1][:4])

    try:
        if identifier == 'cooperation':
            data['mean_coop'] = data['f_c'] / (data['f_d'] + data['f_c'])
            plt.plot(data[['t']].to_numpy(), data[['mean_coop']].to_numpy(), color = colors[index % len(colors)])
            plt.ylim(0., 1.)
            plt.savefig(filename + 'cooperation.png', dpi = 400)

        elif identifier == 'q-table':
            plot_separate_column(colnames, colors)
    except:
        print('Erro, verificar\n')

    plt.clf()
    plt.cla()
    plt.close()

path = './data/'

colors   = ['red', 'blue', 'cyan', 'black', 'green', 'grey', 'hotpink']

cooperation_dict = {}
colnames_dynamic = ['t',  'f_c',  'f_d',  'Qdd',  'Qdc', 'Qdm', 'Qcd', 'Qcc', 'Qcm']
colnames_static = ['t',  'f_c',  'f_d',  'Qdd', 'Qdc', 'Qcd', 'Qcc']

labels_to_plot = []
x_axis_to_plot = []
cooperation_plot = []

index = 0
for filename in glob.glob(path + 'T*.dat'):
    data = pd.read_csv(filename, comment = '#', delimiter = ' ', names = colnames_dynamic, index_col = False)

    key = float(filename.split('rho')[1][0:4])

    data['mean_coop'] = data['f_c'] / (data['f_d'] + data['f_c'])

    """#plot_data_values(filename, data, colnames_dynamic, colors, 'cooperation')
    #plot_data_values(filename, data, colnames_dynamic, colors, 'q-table')

    if 0.8 > key > 0.5:
        plot_data_values(filename, data, colnames_dynamic, colors, 'q-table')
        plot_data_values(filename, data, colnames_dynamic, colors, 'cooperation')
"""
    #plot_data_values(filename, data, colnames_dynamic, colors, 'q-table')
    #plot_data_values(filename, data, colnames_dynamic, colors, 'cooperation')

    x_variable  = float(filename.split('T')[1][:4])
    mean_coop   = np.mean(data[['mean_coop']].to_numpy()[-100:])

    if np.random.rand() < 0.1:
        plot_data_values(filename, data, colnames_dynamic, colors, 'q-table')

    if key in (cooperation_dict.keys()):
        cooperation_dict[key].append([x_variable, float(mean_coop)])
    else:
        cooperation_dict[key] = [[x_variable, float(mean_coop)]]

    labels_to_plot.append(key)
    x_axis_to_plot.append(x_variable)
    cooperation_plot.append(mean_coop)

    index += 1

#plot_heatmap(labels_to_plot, x_axis_to_plot, cooperation_plot)

plt.style.use('ggplot')
color = itertools.cycle(("#0E56FD", "#6135ca", "#ca23dc",  "#e61976", "#d02f6a", "#ff1611"))

marker = itertools.cycle((',', 'P', 'p', '.', '*', 'X', 'P', 'p', 'o'))

index = 0
for key in sorted(cooperation_dict.keys()):
    color_both_plots = next(color)
    plt.scatter(*zip(*cooperation_dict[key]),  marker = next(marker), linestyle='',
        label = r'$\rho = $' + str(key), color = color_both_plots)
    #plt.plot(*zip(*cooperation_dict[key]), linewidth = 0.5, alpha=0.4, color = color_both_plots)
    index += 1

plt.title('')
plt.ylim(0.25, .6)
plt.xlabel(r'$b$')
plt.ylabel(r'$f_c$')
plt.legend()
plt.legend(loc='lower right')
plt.savefig('cooperation_versus_b-per_occupation.png', dpi=400, bbox_inches='tight')

plt.close()
plt.clf()
plt.cla()

"""index = 0
for filename in glob.glob(path + 'T*.dat'):
    data = pd.read_csv(filename, comment = '#', delimiter = ' ', names = colnames_dynamic, index_col = False)
    plot_data_values(filename, data, colnames_dynamic, colors, 'q-table')
    index += 1"""

'''
#print(temptation, mean_cooperation)
plt.plot(*zip(*cooperation_per_episode), label = str(temptation), color = colors[index % len(colors)])
index += 1

plt.savefig('cooperation-time_series.png', dpi = 400, bbox_inches='tight')
plt.clf()'''
