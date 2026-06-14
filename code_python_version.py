import sys

# Constants
AVE_CON_PER_PERSON = 200
AVE_CON_PER_SQU = 9
HEATER = 550
GLOBAL_COLUMN = 35

# Usage patterns (enum)
class Use:
    ONCE = 1
    DAILY = 2
    MO_FR = 3   # Monday to Friday
    SA_SU = 4   # Saturday and Sunday
    WEEKLY = 5

use_map = {
    'o': Use.ONCE,
    'd': Use.DAILY,
    'm': Use.MO_FR,
    's': Use.SA_SU,
    'w': Use.WEEKLY,
}

def input_use(prompt):
    """Prompt user for usage pattern, return Use value."""
    print(prompt)
    while True:
        ch = input().strip().lower()
        if ch in use_map:
            return use_map[ch]
        print("Please enter a valid option: d, m, o, s, or w")

# ---------- Consumer (linked list node) ----------
class Consumer:
    def __init__(self, description="", watt=0.0, watt_standby=0.0, hours=0.0, use=Use.ONCE):
        self.description = description
        self.watt = watt
        self.watt_standby = watt_standby
        self.hours = hours
        self.use = use
        self.next = None

# ---------- Household ----------
class Household:
    def __init__(self, city_name, num_sq, num_persons, heater):
        self.name_of_city = city_name
        self.number_of_sq = num_sq
        self.number_of_person = num_persons
        self.consumer_head = None
        self.heater = heater

# ---------- Helper functions ----------
def annual_hours_of_use(consumer):
    if consumer.use == Use.ONCE:
        return consumer.hours
    elif consumer.use == Use.DAILY:
        return consumer.hours * 365
    elif consumer.use == Use.MO_FR:
        return consumer.hours * 260
    elif consumer.use == Use.SA_SU:
        return consumer.hours * 104
    elif consumer.use == Use.WEEKLY:
        return consumer.hours * 52
    return 0

def annual_hours_of_standby(consumer):
    return (365 * 24) - annual_hours_of_use(consumer)

def annual_kwh(consumer):
    use_hours = annual_hours_of_use(consumer)
    standby_hours = annual_hours_of_standby(consumer)
    return (use_hours * consumer.watt + standby_hours * consumer.watt_standby) / 1000.0

def add_consumer_to_household(household, consumer):
    """Insert consumer at head of list."""
    consumer.next = household.consumer_head
    household.consumer_head = consumer

def move_up(head, k):
    """
    Move the k-th consumer (1-indexed) one position up.
    Returns new head of the list.
    """
    if k <= 1 or head is None:
        return head

    # Case k == 2: swap head and second node
    if k == 2:
        second = head.next
        if second is None:
            return head
        head.next = second.next
        second.next = head
        return second

    # Case k >= 3: find node (k-2) and swap (k-1) with k
    prev = head
    # Move (k-3) steps to reach node (k-2)
    for _ in range(k - 3):
        if prev.next is None or prev.next.next is None:
            return head   # k out of range
        prev = prev.next

    node_k_minus_1 = prev.next      # (k-1)-th node
    node_k = node_k_minus_1.next     # k-th node
    if node_k is None:
        return head

    # Swap the two nodes
    prev.next = node_k
    node_k_minus_1.next = node_k.next
    node_k.next = node_k_minus_1

    return head

def print_consumer(consumer, index):
    """Print a single consumer's details."""
    print(f"{'':>{GLOBAL_COLUMN}}{index}: {consumer.description} (at address: {id(consumer)})")
    print(f"{'':>{GLOBAL_COLUMN}}power consumption: {consumer.watt} w")
    print(f"{'':>{GLOBAL_COLUMN}}power consumption standby: {consumer.watt_standby} w")
    print(f"{'':>{GLOBAL_COLUMN}}annual hours of use: {annual_hours_of_use(consumer)} h")

def print_household(household, price_per_kwh, num):
    """Print full household report."""
    heater_str = "Yes" if household.heater else "No"
    print(f"H O U S E H O L D  N O  {num}  P O W E R   C O N S U M P T I O N")
    print("-" * 70)
    print(f"{'':>{GLOBAL_COLUMN}}city: {household.name_of_city} (at address: {id(household)})")
    print(f"{'':>{GLOBAL_COLUMN}}price for one kWh: {price_per_kwh * 100:.2f} ct/kWh")
    print(f"{'':>{GLOBAL_COLUMN}}square meters: {household.number_of_sq} qm")
    print(f"{'':>{GLOBAL_COLUMN}}persons: {household.number_of_person}")
    print(f"{'':>{GLOBAL_COLUMN}}water heated using electricity: {heater_str}")
    print(f"{'':>{GLOBAL_COLUMN}}list of consumers")

    # Base consumption from area and water heating
    if household.heater:
        base_power = (household.number_of_sq * AVE_CON_PER_SQU) + (household.number_of_person * HEATER)
    else:
        base_power = (household.number_of_sq * AVE_CON_PER_SQU) + (household.number_of_person * AVE_CON_PER_PERSON)

    total_power = base_power
    current = household.consumer_head
    i = 1
    print("-" * 70)
    while current:
        total_power += annual_kwh(current)
        print_consumer(current, i)
        print(f"{'':>{GLOBAL_COLUMN}}annual hours of standby: {annual_hours_of_standby(current)} h")
        print(f"{'':>{GLOBAL_COLUMN}}annual consumption: {annual_kwh(current):.2f} kWh")
        print(f"{'':>{GLOBAL_COLUMN}}annual consumption: {annual_kwh(current) * price_per_kwh:.2f} EUR")
        current = current.next
        i += 1

    print("-" * 70)
    print(f"{'':>{GLOBAL_COLUMN}}power consumption square meters: {household.number_of_sq * AVE_CON_PER_SQU} kWh")
    if household.heater:
        print(f"{'':>{GLOBAL_COLUMN}}power consumption all persons: {household.number_of_person * HEATER} kWh")
    else:
        print(f"{'':>{GLOBAL_COLUMN}}power consumption all persons: {household.number_of_person * AVE_CON_PER_PERSON} kWh")
    print(f"{'':>{GLOBAL_COLUMN}}total annual power consumption: {total_power:.2f} kWh")
    print(f"{'':>{GLOBAL_COLUMN}}total annual power costs: {total_power * price_per_kwh:.2f} EUR")

def input_household():
    """Prompt for square meters, persons, heater; return tuple."""
    sq = int(input("How many square meters does the household have? "))
    persons = int(input("How many persons live in this household? "))
    heater_char = input("Is hot water heated using electricity? (y(es) or n(o)) ").strip().lower()
    heater = heater_char == 'y'
    return sq, persons, heater

def copy_consumers(from_house, to_house):
    """
    Copy all consumers from from_house and append to the END of to_house's list.
    Returns the modified to_house.
    """
    if from_house.consumer_head is None:
        return to_house

    # Build a new list of copies (preserving order)
    new_head = None
    new_tail = None
    current = from_house.consumer_head
    while current:
        new_consumer = Consumer(
            description=current.description,
            watt=current.watt,
            watt_standby=current.watt_standby,
            hours=current.hours,
            use=current.use
        )
        if new_head is None:
            new_head = new_consumer
            new_tail = new_consumer
        else:
            new_tail.next = new_consumer
            new_tail = new_consumer
        current = current.next

    # Append the new list to the end of the target list
    if to_house.consumer_head is None:
        to_house.consumer_head = new_head
    else:
        tail = to_house.consumer_head
        while tail.next:
            tail = tail.next
        tail.next = new_head

    return to_house

# ---------- Main ----------
def main():
    print("CALCULATION OF AVERAGE POWER COSTS FOR A HOUSEHOLD")
    n = int(input("how many households does the house have? "))
    households = [None] * n

    city = input("In which city is the household located? ")
    price = float(input("What is the price for one kWh in EUR? "))

    while True:
        print("\nq quit\ni input power consumer\nu move up power consumer\np print household\na print all households\nn new household\nc copy all consumers (added to already existing ones)")
        cmd = input(">> ").strip().lower()

        if cmd == 'q':
            sys.exit(0)

        elif cmd == 'i':
            idx = int(input("number of household? "))
            if 0 <= idx < n and households[idx] is not None:
                desc = input("What is the description of the power consumer? ")
                watt = float(input("How many watts will it have? "))
                watt_standby = float(input("How many watts standby will it have? "))
                use = input_use("\ndaily  (d)\nmo_fr  (m)\nonce   (o)\nsa_su  (s)\nweekly (w)? ")
                hours = float(input("how many hours it will be operating then? "))
                new_consumer = Consumer(desc, watt, watt_standby, hours, use)
                add_consumer_to_household(households[idx], new_consumer)
            else:
                print("household not exist")

        elif cmd == 'u':
            idx = int(input("which household? "))
            pos = int(input("which one? "))   # 1-based as typed by user
            if 0 <= idx < n and households[idx] is not None:
                households[idx].consumer_head = move_up(households[idx].consumer_head, pos)
            else:
                print("household not exist")

        elif cmd == 'p':
            idx = int(input("number of household? "))
            if 0 <= idx < n and households[idx] is not None:
                print_household(households[idx], price, idx)
            else:
                print("household not exist")

        elif cmd == 'n':
            idx = int(input("number of household? "))
            if 0 <= idx < n and households[idx] is None:
                sq, persons, heater = input_household()
                households[idx] = Household(city, sq, persons, heater)
            else:
                print("Household already exists or index out of range")

        elif cmd == 'a':
            for i in range(n):
                if households[i] is not None:
                    print_household(households[i], price, i)
                    print()

        elif cmd == 'c':
            from_idx = int(input("number of household from which to copy consumers? "))
            to_idx = int(input("number of household to copy to? "))
            if (0 <= from_idx < n and 0 <= to_idx < n and
                households[from_idx] is not None and households[to_idx] is not None):
                households[to_idx] = copy_consumers(households[from_idx], households[to_idx])
            else:
                print("Invalid household index or household does not exist")

        else:
            print("Please enter a valid option: q, u, i, p, a, n, c")

if __name__ == "__main__":
    main()
