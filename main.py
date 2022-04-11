import matplotlib.pyplot as plt
import pandas as pd
import geopandas as gpd
import seaborn as sns
from shapely.geometry import Point

LOG_PATH = "OUT.LOG"
GEORGIA_MAP_PATH = "./tl_2019_13_cousub/tl_2019_13_cousub.shp"

def main():
    df = pd.read_csv(LOG_PATH)
    df.set_index(df["t"] * 1E-3)
    sns.set_style("dark")

    fig = plt.figure(constrained_layout=True, figsize=(18, 9))
    ax_dict = fig.subplot_mosaic(
        [
            ["map", "map", "map"],
            ["temp", "alt", "speed"],
        ],
    )

    # plot temperature
    sns.lineplot(data=df[["temp_int", "temp_ext"]], ax=ax_dict["temp"])
    ax_dict["temp"].set_title("Temperature")
    ax_dict["temp"].legend(labels=["Internal", "External"], title="Source")

    # plot altitude
    sns.lineplot(data=df["altitude"], ax=ax_dict["alt"])
    ax_dict["alt"].set_title("Altitude")

    # plot speed
    sns.lineplot(data=df["speed"], ax=ax_dict["speed"])
    ax_dict["speed"].set_title("Speed")

    # plot map
    georgia_map = gpd.read_file(GEORGIA_MAP_PATH)
    long_lat = df[["long", "lat"]]
    non_null_long_lat = long_lat.loc[(long_lat!=0).all(axis=1)]
    geometry = non_null_long_lat.apply(Point, axis=1)
    geo_df = gpd.GeoDataFrame(df, crs=4326, geometry=geometry)
    minx, miny, maxx, maxy = georgia_map.total_bounds
    ax_dict["map"].set_xlim(minx, maxx)
    ax_dict["map"].set_ylim(33, maxy)
    georgia_map.plot(ax=ax_dict["map"], color="pink", zorder=1)
    geo_df.plot(ax=ax_dict["map"], markersize=1, color="black", marker="o", zorder=2)
    ax_dict["map"].set_title("Path")

    plt.show()

if __name__ == "__main__":
    main()
