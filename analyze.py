import pandas as pd
import argparse

def percentile(n):
    def percentile_(x):
        return x.quantile(n)
    percentile_.__name__ = f"p{n * 100.:.2f}"
    return percentile_

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--filename", required = False, default="/tmp/prof", help="Filename of the CSV profiling output")

    args = parser.parse_args()

    pd.set_option('display.float_format', lambda x: '%.2f' % x)
    df = pd.read_csv(args.filename)
    df = df.groupby(["name"]).agg(["sum", "count", "mean", "std", percentile(0.99)])
    df.sort_values([("ticks", "sum")], ascending=False, inplace=True)

    print ("Profiling Results: ")
    print (df)
