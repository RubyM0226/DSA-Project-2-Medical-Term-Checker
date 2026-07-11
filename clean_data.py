# CLAUDE PYTHON FILE THAT HELPED US EXTRACT AND FORMAT OUR DATA

import json

with open('drug-ndc-0001-of-0001.json', encoding='utf-8') as f:
    data = json.load(f)

terms = set()

for entry in data['results']:
    brand = entry.get('brand_name', '').strip().lower()
    generic = entry.get('generic_name', '').strip().lower()
    labeler = entry.get('labeler_name', '').strip().lower()
    dosage = entry.get('dosage_form', '').strip().lower()

    # add standalone names
    if brand: terms.add(brand)
    if generic: terms.add(generic)

    # add combined strings for more unique entries
    if brand and generic and brand != generic:
        terms.add(f"{brand} {generic}")
    if brand and dosage:
        terms.add(f"{brand} {dosage}")
    if generic and dosage:
        terms.add(f"{generic} {dosage}")
    if labeler and brand:
        terms.add(f"{brand} {labeler}")

    # pull active ingredient names too
    for ingredient in entry.get('active_ingredients', []):
        name = ingredient.get('name', '').strip().lower()
        if name: terms.add(name)
        if name and dosage:
            terms.add(f"{name} {dosage}")

with open('terms.txt', 'w', encoding='utf-8') as out:
    for term in sorted(terms):
        out.write(term + '\n')

print(f"Total unique terms: {len(terms)}")