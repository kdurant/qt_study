def read_in_chunks(filePath, chunk_size=1024 * 1024):
    """
    Lazy function (generator) to read a file piece by piece.
    Default chunk size: 1M
    You can set your own chunk size 
    """
    file_object = open(filePath, "rb")
    while True:
        chunk_data = file_object.read(chunk_size)
        if not chunk_data:
            break
        yield chunk_data


if __name__ == "__main__":
    filePath = '../big_flare_radar.bin'
    for chunk in read_in_chunks(filePath, 1024 * 1024):
        print((chunk))
