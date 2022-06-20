import dash
from dash import dcc, html
import dash_bootstrap_components as dbc
from plotly.subplots import make_subplots
import plotly.graph_objects as go
import dash_leaflet as dl
import plotly.express as px

import pandas as pd

external_scripts = []

external_stylesheets = ['https://codepen.io/chriddyp/pen/bWLwgP.css', dbc.themes.BOOTSTRAP, 'style.css']

app = dash.Dash(__name__, external_stylesheets=external_stylesheets, external_scripts=external_scripts)
server = app.server

patterns = []
df = pd.read_csv("LOG00194.csv")
df['temperature_outside'] = df['temperature_outside'].astype(float)
df['temperature_outside'] = df['temperature_outside'].round()

coords = df[['lattitude', 'longitude']].values.tolist()
heating = df[df['heating'] == 1].count()['heating']

polyline = dl.Polyline(positions=coords)
marker_pattern = dl.PolylineDecorator(children=polyline, patterns=patterns)
# custom marker

'''@app.callback(Output('live-update-text1', 'children'),
              Input('interval-component', 'n_intervals'))'''


def update_metrics():
    df = pd.read_csv("LOG00194.csv")
    lon = df['current'].max()
    lat = df['voltage'].max()
    alt = df['intensivity'].max()
    bright = df['brightness'].max()
    style = {'padding': '5px', 'fontSize': '16px'}
    return [
        dbc.Card(
            [
                dbc.CardHeader("Максимальное значение тока"),
                dbc.CardBody(
                    [
                        html.H4(f'{lon} мА', className="card-title"),
                    ]
                ),
            ],
            style={"width": "18rem", 'margin': '16px', 'margin-top': '0px'},
        ),
        dbc.Card(
            [
                dbc.CardHeader("Максимальное напряжение"),
                dbc.CardBody(
                    [
                        html.H4(f'{lat} мВ', className="card-title"),
                    ]
                ),
            ],
            style={"width": "18rem", 'margin': '16px'},
        ),
        dbc.Card(
            [
                dbc.CardHeader("Максимальная освещенность"),
                dbc.CardBody(
                    [
                        html.H4(f'{alt} МилиВт/см2', className="card-title"),
                    ]
                ),
            ],
            style={"width": "18rem", 'margin': '16px'},
        ),
        dbc.Card(
            [
                dbc.CardHeader("Максимальная яркость"),
                dbc.CardBody(
                    [
                        html.H4(f'{bright} Люкс', className="card-title"),
                    ]
                ),
            ],
            style={"width": "18rem", 'margin': '16px', 'margin-bottom': '0px'},
        )
    ]


'''@app.callback(Output('live-update-pie', 'figure'),
              Input('interval-component', 'n_intervals'))'''


def update_pie_live():
    df = pd.read_csv("LOG00194.csv")
    labels = ['LPG', 'CH4', 'CO', 'H2', 'CO2']
    values = [df['LPG'].iloc[3].tolist(), df['CH4'].iloc[3].tolist(), df['CO'].iloc[3].tolist(), df['H2'].iloc[3].tolist(),
              df['CO2'].iloc[3].tolist()]
    fig = go.Figure(data=[go.Pie(labels=labels, values=values)])
    fig.update_layout(height=285)
    return fig


'''@app.callback(Output('live-update-pie1', 'figure'),
              Input('interval-component', 'n_intervals'))'''


def update_pie_live1():
    df = pd.read_csv("LOG00194.csv")
    labels = ['LPG', 'CH4', 'CO', 'H2', 'CO2']
    values = [df['LPG'].iloc[679].tolist(), df['CH4'].iloc[679].tolist(), df['CO'].iloc[679].tolist(), df['H2'].iloc[679].tolist(),
              df['CO2'].iloc[679].tolist()]
    fig = go.Figure(data=[go.Pie(labels=labels, values=values)])
    fig.update_layout(height=285)
    return fig


'''@app.callback(Output('live-update-text', 'children'),
              Input('interval-component', 'n_intervals'))'''


def update_metrics1():
    df = pd.read_csv("LOG00194.csv")
    electronics_min = df['temperature_electronics'].min()
    electronics_max = df['temperature_electronics'].max()
    battery_min = df['temperature_battery'].min()
    battery_max = df['temperature_battery'].max()
    pres_sens_max = df['temperature_pres_sens'].max()
    pres_sens_min = df['temperature_pres_sens'].min()
    return [
        dbc.Card(
            [
                dbc.CardHeader("Температура внутри"),
                dbc.CardBody(
                    [
                        html.H4(f'{electronics_min}/{electronics_max} °С', className="card-title"),
                        html.P("Минимум/максимум", className="card-text"),
                    ]
                ),
            ],
            style={"width": "15rem", 'margin': '9px', 'margin-top': '15px'},
        ),
        dbc.Card(
            [
                dbc.CardHeader("Температура аккумулятора"),
                dbc.CardBody(
                    [
                        html.H4(f'{battery_min}/{battery_max} °С', className="card-title"),
                        html.P("Минимум/максимум", className="card-text"),
                    ]
                ),
            ],
            style={"width": "15rem", 'margin': '9px'},
        ),
        dbc.Card(
            [
                dbc.CardHeader("Температура снаружи"),
                dbc.CardBody(
                    [
                        html.H4(f'-45.68/27.56 °С', className="card-title"),
                        html.P("Минимум/максимум", className="card-text"),
                    ]
                ),
            ],
            style={"width": "15rem", 'margin': '9px'},
        ),
        dbc.Card(
            [
                dbc.CardHeader("Температура на барометре"),
                dbc.CardBody(
                    [
                        html.H4(f'{pres_sens_min}/{pres_sens_max} °С', className="card-title"),
                        html.P("Минимум/максимум", className="card-text"),
                    ]
                ),
            ],
            style={"width": "15rem", 'margin': '9px', 'margin-bottom': '0px'},
        )
    ]


# Multiple components can update everytime interval gets fired.
'''@app.callback(Output('live-update-graph', 'figure'),
              Input('interval-component', 'n_intervals'))'''


def update_graph_live():
    df = pd.read_csv("LOG00194.csv")
    # Create the graph with subplots
    fig = make_subplots(rows=1, cols=1)
    fig['layout']['margin'] = {
        'l': 30, 'r': 10, 'b': 30, 't': 10
    }
    fig['layout']['legend'] = {'x': 0, 'y': 1, 'xanchor': 'left'}
    fig.append_trace({
        'x': df['time'].to_list(),
        'y': df['altitude_GPS'].to_list(),
        'name': 'Altitude',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.append_trace({
        'x': df['time'].to_list(),
        'y': df['altitude_pres_sens'].to_list(),
        'name': 'altitude_pres_sens',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.update_layout(height=125)
    return fig


'''@app.callback(Output('live-update-graph1', 'figure'),
              Input('interval-component', 'n_intervals'))'''


def update_graph_live1():
    df = pd.read_csv("LOG00194.csv")
    fig = make_subplots(rows=1, cols=1)
    fig['layout']['margin'] = {
        'l': 30, 'r': 10, 'b': 30, 't': 10
    }
    fig['layout']['legend'] = {'x': 0, 'y': 1, 'xanchor': 'left'}

    fig.append_trace({
        'x': df['time'].to_list(),
        'y': df['radiation_dinamic'].to_list(),
        'text': df['time'].to_list(),
        'name': 'Longitude vs Latitude',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.update_layout(height=125)
    return fig


# Multiple components can update everytime interval gets fired.
''' @app.callback(Output('live-update-graph2', 'figure'),
              Input('interval-component', 'n_intervals'))'''


def update_graph_live2():
    df = pd.read_csv("LOG00194.csv")

    # Create the graph with subplots
    fig = make_subplots(rows=1, cols=1)
    fig['layout']['margin'] = {
        'l': 30, 'r': 10, 'b': 30, 't': 10
    }
    fig['layout']['legend'] = {'x': 0, 'y': 1, 'xanchor': 'left'}
    fig.append_trace({
        'x': df['time'].to_list(),
        'y': df['temperature_pres_sens'].to_list(),
        'name': 'Барометр',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.append_trace({
        'x': df['time'].to_list(),
        'y': df['temperature_electronics'].to_list(),
        'name': 'Внутри',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.append_trace({
        'x': df['time'].to_list(),
        'y': df['temperature_battery'].to_list(),
        'name': 'На батарее',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.append_trace({
        'x': df['time'].to_list(),
        'y': df['temperature_outside'].to_list(),
        'name': 'Снаружи',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.update_layout(height=125)
    return fig


# Multiple components can update everytime interval gets fired.
'''@app.callback(Output('live-update-graph3', 'figure'),
              Input('interval-component', 'n_intervals'))'''


def update_graph_live3():
    df = pd.read_csv("LOG00194.csv")
    # Create the graph with subplots
    fig = make_subplots(rows=1, cols=1)
    fig['layout']['margin'] = {
        'l': 30, 'r': 10, 'b': 30, 't': 10
    }
    fig['layout']['legend'] = {'x': 0, 'y': 1, 'xanchor': 'left'}
    fig.append_trace({
        'x': df['altitude_GPS'].iloc[:678].to_list(),
        'y': df['pressure'].iloc[:678].to_list(),
        'name': 'по GPS',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.append_trace({
        'x': df['altitude_pres_sens'].iloc[:678].to_list(),
        'y': df['pressure'].iloc[:678].to_list(),
        'name': 'по барометру',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.update_layout(height=125)
    return fig


def update_graph_live4():
    df = pd.read_csv("LOG00194.csv")
    # Create the graph with subplots
    fig = make_subplots(rows=1, cols=1)
    fig['layout']['margin'] = {
        'l': 30, 'r': 10, 'b': 30, 't': 10
    }
    fig['layout']['legend'] = {'x': 0, 'y': 1, 'xanchor': 'left'}
    fig.append_trace({
        'x': df['altitude_GPS'].iloc[:678].to_list(),
        'y': df['radiation_dinamic'].iloc[:678].to_list(),
        'name': 'Altitude',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.append_trace({
        'x': df['altitude_pres_sens'].iloc[:678].to_list(),
        'y': df['radiation_dinamic'].iloc[:678].to_list(),
        'name': 'Altitude',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.update_layout(height=125)
    return fig


def update_graph_live5():
    df = pd.read_csv("LOG00194.csv")
    # Create the graph with subplots
    fig = make_subplots(rows=1, cols=1)
    fig['layout']['margin'] = {
        'l': 30, 'r': 10, 'b': 30, 't': 10
    }
    fig['layout']['legend'] = {'x': 0, 'y': 1, 'xanchor': 'left'}
    fig.append_trace({
        'x': df['altitude_GPS'].iloc[:678].to_list(),
        'y': df['temperature_pres_sens'].iloc[:678].to_list(),
        'name': 'По барометру',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.append_trace({
        'x': df['altitude_GPS'].iloc[:678].to_list(),
        'y': df['temperature_electronics'].iloc[:678].to_list(),
        'name': 'Внутри',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.append_trace({
        'x': df['altitude_GPS'].iloc[:678].to_list(),
        'y': df['temperature_battery'].iloc[:678].to_list(),
        'name': 'Батарея',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.append_trace({
        'x': df['altitude_GPS'].iloc[:678].to_list(),
        'y': df['temperature_outside'].iloc[:678].to_list(),
        'name': 'Снаружи',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.update_layout(height=125)
    return fig


def update_graph_live6():
    df = pd.read_csv("LOG00194.csv")
    # Create the graph with subplots
    fig = make_subplots(rows=1, cols=1)
    fig['layout']['margin'] = {
        'l': 30, 'r': 10, 'b': 30, 't': 10
    }
    fig['layout']['legend'] = {'x': 0, 'y': 1, 'xanchor': 'left'}
    fig.append_trace({
        'x': df['time'].to_list(),
        'y': df['heating'].to_list(),
        'name': 'Altitude',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.update_layout(height=125)
    return fig


def update_graph_live7():
    df = pd.read_csv("LOG00194.csv")
    # Create the graph with subplots
    fig = make_subplots(rows=1, cols=1)
    fig['layout']['margin'] = {
        'l': 30, 'r': 10, 'b': 30, 't': 10
    }
    fig['layout']['legend'] = {'x': 0, 'y': 1, 'xanchor': 'left'}
    fig.append_trace({
        'x': df['altitude_GPS'].iloc[:678].to_list(),
        'y': df['intensivity'].iloc[:678].to_list(),
        'name': 'Altitude',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.update_layout(height=125)
    return fig


def update_graph_live8():
    df = pd.read_csv("LOG00194.csv")
    # Create the graph with subplots
    fig = make_subplots(rows=1, cols=1)
    fig['layout']['margin'] = {
        'l': 30, 'r': 10, 'b': 30, 't': 10
    }
    fig['layout']['legend'] = {'x': 0, 'y': 1, 'xanchor': 'left'}
    fig.append_trace({
        'x': df['time'].to_list(),
        'y': df['pressure'].to_list(),
        'name': 'Altitude',
        'mode': 'lines',
        'type': 'scatter'
    }, 1, 1)
    fig.update_layout(height=125)
    return fig


def make_histogram():
    df = pd.read_csv("LOG00194.csv")

    df_alt_GPS = df[['altitude_GPS', 'time']]
    df_alt_GPS = df_alt_GPS.assign(type='GPS')
    df_alt_GPS = df_alt_GPS.rename(columns={'altitude_GPS': 'altitude'})

    df_alt_pres = df[['altitude_pres_sens', 'time']]
    df_alt_pres = df_alt_pres.assign(type='pres_sens')
    df_alt_pres = df_alt_pres.rename(columns={'altitude_pres_sens': 'altitude'})

    merged = pd.concat([df_alt_GPS, df_alt_pres])

    fig = px.histogram(merged, x='time', y='altitude', color='type', nbins=40, barmode="group", height=488, histfunc='avg')

    return fig


cards = html.Div(
    [
        html.Div(
            [html.Div(dbc.Card(
                [dbc.CardHeader("Высота от времени"),
                 dbc.CardBody(
                     [
                            dcc.Graph(figure=update_graph_live())
                     ]
                 ), ],
                className="mb-3", ), style={'margin': '10px', 'width': '650px', 'margin-top': '15px', 'margin-left': '45px', 'margin-right': '0px'}
            ),
                html.Div(
                    dbc.Card([dbc.CardHeader("Радиация от времени"),
                              dbc.CardBody(
                                  [
                                      dcc.Graph(figure=update_graph_live1())
                                  ]
                              )],
                             className="",
                             ), style={'margin': '10px', 'margin-left': '45px', 'margin-right': '0px'}),
                html.Div(dbc.Card(
                    [dbc.CardHeader("Температура от времени"), dbc.CardBody(
                        [
                            dcc.Graph(figure=update_graph_live2())
                        ]
                    ), ],
                    className="mb-3",
                ), style={'margin': '10px', 'margin-left': '45px', 'margin-right': '0px'}),
                html.Div(dbc.Card(
                    [dbc.CardHeader("Давление от высоты"), dbc.CardBody(
                        [
                            dcc.Graph(figure=update_graph_live3())
                        ]
                    ), ]
                    ,
                    className="mb-3",
                ), style={'margin': '10px', 'margin-left': '45px', 'margin-right': '0px'})], style={'float': 'left'}),
        html.Div([html.Div(update_metrics1(), style={'display': 'block', 'float': 'left', 'margin-left': '18px'}),
                  html.Div(dl.Map(center=[56.752016, 39.449476], zoom=9, children=[dl.TileLayer(), marker_pattern,
                                                                                   dl.Marker(
                                                                                       position=[56.920100, 39.003164]),
                                                                                   dl.Marker(
                                                                                       position=[56.634232, 39.132600]),
                                                                                   # dl.MeasureControl(position="topleft",
                                                                                   #                  primaryLengthUnit="kilometers",
                                                                                   #                  primaryAreaUnit="hectares",
                                                                                   #                  activeColor="#214097",
                                                                                   #                  completedColor="#972158"),
                                                                                   dl.FeatureGroup([
                                                                                       dl.EditControl(
                                                                                           id="edit_control")]),
                                                                                   ]),
                           style={"width": "38%", "height": "370px", 'display': 'block', 'float': 'left', 'margin':
                               '19px', 'margin-top': '15px'})]),
        html.Div(dbc.Card(
            [dbc.CardHeader('Состав воздуха на земле'), dbc.CardBody(
                [
                    dcc.Graph(figure=update_pie_live())
                ]
            ), ],
            className="mb-3", ), style={'float': 'left', 'width': '18%', 'margin-left': '27px', 'margin-right': '19px'}
        ),
        html.Div(dbc.Card(
            [dbc.CardHeader('Состав воздуха на максимальной высоте'), dbc.CardBody(
                [
                    dcc.Graph(figure=update_pie_live1())
                ]
            ), ],
            className="mb-3", ), style={'float': 'left', 'width': '18%'}
        ),
        html.Div(update_metrics(), style={'display': 'block', 'float': 'left'}),
        html.Div([html.Div(dbc.Card(
            [dbc.CardHeader('Гистограмма высот с барометра и GPS'), dbc.CardBody(
                [
                    dcc.Graph(figure=make_histogram())
                ]
            ), ],
            className="mb-3", ), style={'float': 'left', 'display': 'block', 'width': '49%', 'margin-left': '45px', 'margin-right': '19px', 'margin-top': '15px'}
        )
        ,
        html.Div(
            [html.Div(dbc.Card(
                [dbc.CardHeader("Радиация от высоты"),
                 dbc.CardBody(
                     [
                            dcc.Graph(figure=update_graph_live4())
                     ]
                 ), ],
                className="mb-3", ), style={'margin': '10px', 'width': '650px', 'margin-top': '15px'}
            ),
                html.Div(
                    dbc.Card([dbc.CardHeader("Температура от высоты по GPS"),
                              dbc.CardBody(
                                  [
                                      dcc.Graph(figure=update_graph_live5())
                                  ]
                              )],
                             className="",
                             ), style={'margin': '10px'}),
                html.Div(dbc.Card(
                    [dbc.CardHeader("Включение нагревателя"), dbc.CardBody(
                        [
                            dcc.Graph(figure=update_graph_live6())
                        ]
                    ), ],
                    className="mb-3",
                ), style={'margin': '10px'}),
                ],
            style={'float': 'right', 'margin-right': '30px', 'margin-bottom': '0px'})], style={'display': 'block'}),
            html.Div([html.Div([html.Div(dbc.Card(
                    [dbc.CardHeader("Интенсивность от высоты по GPS"), dbc.CardBody(
                        [
                            dcc.Graph(figure=update_graph_live7())
                        ]
                    ), ]
                    ,
                    className="mb-3",
                ), style={'margin': '10px', 'width': '745px', 'float': 'left', 'margin-left': '45px'})
            ]),
                html.Div(dbc.Card(
                    [dbc.CardHeader("Давление от времени"), dbc.CardBody(
                        [
                            dcc.Graph(figure=update_graph_live8())
                        ]
                    ), ]
                    ,
                    className="mb-3",
                ), style={'margin': '10px', 'width': '650px', 'float': 'right', 'margin-left': '0px', 'margin-right': '40px'})
            ], style={})
        # html.Div(dash.dash_table.DataTable(df.to_dict('records'), [{"name": i, "id": i} for i in df.columns])),
    ],
)
app.layout = html.Div(
    html.Div([
        html.Header('UG Satellite Feed', style={'background-color': '#24292f', 'color': '#ffffff', 'text-indent': '20px', 'height': '25px',
                                                  'display': 'flex', 'justify-content': 'left', 'align-items': 'center', 'font-size': '15px'}),
        cards,
        html.Footer('© Copyright. All rights reserved.', style={"clear": "both",'background-color': '#24292f', 'color': '#ffffff', 'text-indent': '20px', 'height': '50px',
                                                  'font-size': '10px', 'display': 'flex', 'justify-content': 'center', 'align-items': 'center'})
    ])

)


def dump_coords_to_json():
    df = pd.read_csv("LOG00194.csv")
    df = df[['lattitude', 'longitude']]
    with open('formatted.json', 'w') as outfile:
        df.to_json(outfile)
    return


if __name__ == '__main__':
    app.run_server(debug=True)
