from absl import app
from absl import flags
import csv
import numpy as np
import h5py
import hdf5plugin
import cv2
from eventslicer import EventSlicer

FLAGS = flags.FLAGS
flags.DEFINE_string("h5_events", None, "The h5 file with the events.")
flags.DEFINE_string("exposure_timestamps", None, "The file with the image exposure timestamps.")
flags.DEFINE_integer("height", None, "The height of the camera (y resolution).")
flags.DEFINE_integer("width", None, "The width of the camera (x resolution).")

# Required flag.
flags.mark_flag_as_required("h5_events")
flags.mark_flag_as_required("exposure_timestamps")
flags.mark_flag_as_required("height")
flags.mark_flag_as_required("width")


def main(argv):
    del argv  # Unused.

    h5_events = h5py.File(FLAGS.h5_events, 'r')
    event_slicer = EventSlicer(h5_events)

    with open(FLAGS.exposure_timestamps, newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter=',', quotechar='|')
        for row_index, row in enumerate(reader):
            if row_index > 1:
                print("row index: " + str(row_index))
                print(', '.join(row))

                events = event_slicer.get_events(int(row[0]), int(row[1]))
                if events:
                    accumulated_events = np.zeros((FLAGS.height, FLAGS.width, 3), dtype=np.uint8)
                    for x, y, t, p in zip(events['x'], events['y'], events['t'], events['p']):
                        if p > 0:
                            accumulated_events[y, x, 2] = 255
                        else:
                            accumulated_events[y, x, 0] = 255

                    cv2.imwrite("./accumulated_events/accumulated_events_" + str(row_index).zfill(4) + ".png", accumulated_events)


if __name__ == '__main__':
    app.run(main)
