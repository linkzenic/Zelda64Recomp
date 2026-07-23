import os
import sys
import traceback


def generate(yaml_dir, output_dir):
    previous_cwd = os.getcwd()
    previous_argv = sys.argv[:]

    try:
        os.makedirs(output_dir, exist_ok=True)
        os.chdir(output_dir)
        sys.argv = [
            "MMGenerate.py",
            "--player_files_path",
            yaml_dir,
            "--outputpath",
            output_dir,
        ]

        from MMGenerate import main
        main()
        return True
    except Exception:
        traceback.print_exc()
        return False
    finally:
        sys.argv = previous_argv
        os.chdir(previous_cwd)
