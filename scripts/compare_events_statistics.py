import numpy as np
import cv2

from absl import app
from absl import flags

FLAGS = flags.FLAGS
flags.DEFINE_string("statistics_real", None, "The json file with the statistics of the real events.")
flags.DEFINE_string("statistics_sim", None, "The json file with the statistics of the simulated events.")

# Required flag.
flags.mark_flag_as_required("statistics_real")
flags.mark_flag_as_required("statistics_sim")

def main(argv):
    fs = cv2.FileStorage(FLAGS.statistics_real, cv2.FILE_STORAGE_READ)

    total_events_per_pixel_real = fs.getNode('total_events_per_pixel').mat()
    pos_events_per_pixel_real = fs.getNode('pos_events_per_pixel').mat()
    neg_events_per_pixel_real = fs.getNode('neg_events_per_pixel').mat()
    total_events_per_pixel_per_second_real = fs.getNode('total_events_per_pixel_per_second').mat()
    pos_events_per_pixel_per_second_real = fs.getNode('pos_events_per_pixel_per_second').mat()
    neg_events_per_pixel_per_second_real = fs.getNode('neg_events_per_pixel_per_second').mat()
    print("events_per_pixel_real.shape: " + str(total_events_per_pixel_real.shape))
    print("events_per_pixel_per_second_real.shape: " + str(total_events_per_pixel_per_second_real.shape))

    fs = cv2.FileStorage(FLAGS.statistics_sim, cv2.FILE_STORAGE_READ)

    total_events_per_pixel_sim = fs.getNode('total_events_per_pixel').mat()
    pos_events_per_pixel_sim = fs.getNode('pos_events_per_pixel').mat()
    neg_events_per_pixel_sim = fs.getNode('neg_events_per_pixel').mat()
    total_events_per_pixel_per_second_sim = fs.getNode('total_events_per_pixel_per_second').mat()
    pos_events_per_pixel_per_second_sim = fs.getNode('pos_events_per_pixel_per_second').mat()
    neg_events_per_pixel_per_second_sim = fs.getNode('neg_events_per_pixel_per_second').mat()
    print("events_per_pixel_sim.shape: " + str(total_events_per_pixel_sim.shape))
    print("events_per_pixel_per_second_sim.shape: " + str(total_events_per_pixel_per_second_sim.shape))

    print("events_per_pixel_real.shape: " + str(total_events_per_pixel_real.shape))
    print("events_per_pixel_sim.shape: " + str(total_events_per_pixel_sim.shape))

    print("events_per_pixel_per_second_real.shape: " + str(total_events_per_pixel_per_second_real.shape))
    print("events_per_pixel_per_second_sim.shape: " + str(total_events_per_pixel_per_second_sim.shape))

    mean_total_events_per_pixel_sim = np.mean(total_events_per_pixel_sim)
    std_total_events_per_pixel_sim = np.std(total_events_per_pixel_sim)
    var_total_events_per_pixel_sim = np.var(total_events_per_pixel_sim)

    print("total_events_per_pixel_sim:")
    print(" mean: " + str(mean_total_events_per_pixel_sim))
    print(" std: " + str(std_total_events_per_pixel_sim))
    print(" var: " + str(var_total_events_per_pixel_sim))

    mean_total_events_per_pixel_real = np.mean(total_events_per_pixel_real)
    std_total_events_per_pixel_real = np.std(total_events_per_pixel_real)
    var_total_events_per_pixel_real = np.var(total_events_per_pixel_real)

    print("total_events_per_pixel_real:")
    print(" mean: " + str(mean_total_events_per_pixel_real))
    print(" std: " + str(std_total_events_per_pixel_real))
    print(" var: " + str(var_total_events_per_pixel_real))

    mean_total_events_per_pixel_per_second_sim = np.mean(total_events_per_pixel_per_second_sim)
    std_total_events_per_pixel_per_second_sim = np.std(total_events_per_pixel_per_second_sim)
    var_total_events_per_pixel_per_second_sim = np.var(total_events_per_pixel_per_second_sim)

    print("total_events_per_pixel_per_second_sim:")
    print(" mean: " + str(mean_total_events_per_pixel_per_second_sim))
    print(" std: " + str(std_total_events_per_pixel_per_second_sim))
    print(" var: " + str(var_total_events_per_pixel_per_second_sim))

    mean_total_events_per_pixel_per_second_real = np.mean(total_events_per_pixel_per_second_real)
    std_total_events_per_pixel_per_second_real = np.std(total_events_per_pixel_per_second_real)
    var_total_events_per_pixel_per_second_real = np.var(total_events_per_pixel_per_second_real)

    print("total_events_per_pixel_per_second_real:")
    print(" mean: " + str(mean_total_events_per_pixel_per_second_real))
    print(" std: " + str(std_total_events_per_pixel_per_second_real))
    print(" var: " + str(var_total_events_per_pixel_per_second_real))

if __name__ == '__main__':
    app.run(main)
