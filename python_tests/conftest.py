
import secrets
import subprocess

import pytest


def run_tso_command(command: str):
    subprocess.run(
        f'tsocmd "{command}"', 
        text=False, 
        shell=True, 
        check=True, 
        capture_output=True,
        )

@pytest.fixture
def delete_user():
    userid=f"SEAR{secrets.token_hex(2)}".upper()
    yield userid
    try:  # noqa: SIM105
        run_tso_command(f"deluser {userid}")
    except:  # noqa: E722
        pass

@pytest.fixture
def create_user(delete_user):
    run_tso_command(f"ADDUSER {delete_user} DATA('USER GENERATED DURING SEAR TESTING, NOT IMPORTANT')")  # noqa: E501
    yield delete_user

@pytest.fixture
def delete_group():
    groupid=f"SEAR{secrets.token_hex(2)}".upper()
    yield groupid
    try:  # noqa: SIM105
        run_tso_command(f"DELGROUP {groupid}")
    except:  # noqa: E722
        pass

@pytest.fixture
def create_group(delete_group):
    run_tso_command(f"ADDGROUP {delete_group} DATA('GROUP GENERATED DURING SEAR TESTING, NOT IMPORTANT')")  # noqa: E501
    yield delete_group

@pytest.fixture
def delete_dataset():
    profile_name=f"SEARTEST.TEST{secrets.token_hex(2)}.**".upper()
    yield profile_name
    try:  # noqa: SIM105
        run_tso_command(f"DELDSD ({profile_name})")
    except:  # noqa: E722
        pass

@pytest.fixture
def create_dataset(delete_dataset):
    run_tso_command(f"ADDSD ('{delete_dataset}') DATA('DATASET PROFILE GENERATED DURING SEAR TESTING, NOT IMPORTANT') OWNER(SYS1)")  # noqa: E501
    run_tso_command("SETROPTS GENERIC(DATASET) REFRESH")
    yield delete_dataset

@pytest.fixture
def delete_resource():
    profile_name=f"SEARTEST.JUNK{secrets.token_hex(2)}.**".upper()
    class_name = "FACILITY"
    yield profile_name, class_name
    try:  # noqa: SIM105
        run_tso_command(f"RDELETE {class_name} ({profile_name})")
    except:  # noqa: E722
        pass

@pytest.fixture
def create_resource(delete_resource):
    profile_name, class_name = delete_resource
    run_tso_command(f"RDEFINE {class_name} {profile_name} DATA('RESOURCE PROFILE GENERATED DURING SEAR TESTING, NOT IMPORTANT') OWNER(SYS1) FGENERIC")  # noqa: E501
    run_tso_command(f"SETROPTS GENERIC({class_name}) REFRESH")
    run_tso_command(f"SETROPTS RACLIST({class_name}) REFRESH")
    yield profile_name, class_name

@pytest.fixture
def delete_keyring():
    ring_name=f"SEARTEST.RING{secrets.token_hex(2)}".upper()
    owner = "SEARTEST"
    yield ring_name, owner
    try:  # noqa: SIM105
        run_tso_command(f"RACDCERT DELRING({ring_name}) ID({owner})")
        run_tso_command("SETROPTS RACLIST(DIGTRING) REFRESH")
    except:  # noqa: E722
        pass

@pytest.fixture
def create_keyring(delete_keyring):
    ring_name, owner = delete_keyring
    run_tso_command(f"RACDCERT ADDRING({ring_name}) ID({owner})")  # noqa: E501
    run_tso_command("SETROPTS RACLIST(DIGTRING) REFRESH")
    yield ring_name, owner

