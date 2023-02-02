import numpy as np
import cv2
import h5py
import hdf5plugin
from absl import app
from absl import flags

FLAGS = flags.FLAGS
flags.DEFINE_string("h5_events", None, "The h5 file with the events.")
flags.DEFINE_string("h5_rectify", None, "The h5 file with the rectify map.")
flags.DEFINE_float(
    "seconds", 0,
    "The duration [seconds] for which events should be evaluated.")
flags.DEFINE_float("ms", 0,
                   "The duration [ms] for which events should be evaluated.")
flags.DEFINE_integer("height", None, "The height of the camera (y resolution).")
flags.DEFINE_integer("width", None, "The width of the camera (x resolution).")

# Required flag.
flags.mark_flag_as_required("h5_events")
flags.mark_flag_as_required("height")
flags.mark_flag_as_required("width")


def main(argv):
    del argv  # Unused.

    total_events_per_pixel = np.zeros((FLAGS.height, FLAGS.width))
    pos_events_per_pixel = np.zeros((FLAGS.height, FLAGS.width))
    neg_events_per_pixel = np.zeros((FLAGS.height, FLAGS.width))

    # Load events from h5 (until t_end)
    h5_events = h5py.File(FLAGS.h5_events, 'r')
    keys = list(h5_events.keys())
    print("keys: " + str(keys))

    if FLAGS.h5_rectify:
        h5_rectify_map = h5py.File(FLAGS.h5_rectify, 'r')

        keys = list(h5_rectify_map.keys())
        print("keys: " + str(keys))

        rectify_map = h5_rectify_map['rectify_map']
        offset = h5_events['t_offset']

    for event in h5_events['events']:
        print("event: " + str(event))

    duration = float('inf')
    if FLAGS.seconds > 0 or FLAGS.ms > 0:
        duration = 0
        if FLAGS.seconds > 0:
            duration += FLAGS.seconds * 1E6
        if FLAGS.ms > 0:
            duration += FLAGS.ms * 1E3
    print("Duration: " + str(duration) + "us")
    t_end = h5_events['events']['t'][0] + duration
    print("t_end: " + str(t_end))

    for p, t, x, y in zip(h5_events['events']['p'], h5_events['events']['t'],
                          h5_events['events']['x'], h5_events['events']['y']):

        if t > t_end:
            print("End of duration reached! Last timestamp: " + str(t + h5_events['t_offset']))
            break

        # Rectify
        if FLAGS.h5_rectify:
            x, y = rectify_map[y, x]
            x = int(round(x))
            y = int(round(y))

        total_events_per_pixel[y, x] += 1

        if p > 0:
            pos_events_per_pixel[y, x] += 1
        else:
            neg_events_per_pixel[y, x] += 1

    cv2.imwrite("events_per_pixel_from_dsec.png", total_events_per_pixel)
    print("events_per_pixel.shape: " + str(total_events_per_pixel.shape))
    fs = cv2.FileStorage("events_per_pixel_from_dsec.json", cv2.FILE_STORAGE_WRITE)
    fs.write("total_events_per_pixel", total_events_per_pixel)
    fs.write("pos_events_per_pixel", pos_events_per_pixel)
    fs.write("neg_events_per_pixel", neg_events_per_pixel)

    seconds = duration * 1E-6
    total_events_per_pixel_per_second = cv2.multiply(total_events_per_pixel, 1 / seconds)
    pos_events_per_pixel_per_second = cv2.multiply(pos_events_per_pixel, 1 / seconds)
    neg_events_per_pixel_per_second = cv2.multiply(neg_events_per_pixel, 1 / seconds)
    cv2.imwrite("events_per_pixel_per_second_from_dsec.png", total_events_per_pixel_per_second)

    fs.write("total_events_per_pixel_per_second", total_events_per_pixel_per_second)
    fs.write("pos_events_per_pixel_per_second", pos_events_per_pixel_per_second)
    fs.write("neg_events_per_pixel_per_second", neg_events_per_pixel_per_second)
    fs.release()


if __name__ == '__main__':
    app.run(main)
